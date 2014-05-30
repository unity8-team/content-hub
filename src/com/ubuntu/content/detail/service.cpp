/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

// NEEDED for libnotify include
#define QT_NO_KEYWORDS

#include "debug.h"
#include "service.h"
#include "peer_registry.h"
#include "i18n.h"
#include "transfer.h"
#include "transferadaptor.h"
#include "utils.cpp"
#include "ContentHandlerInterface.h"

#include <glib.h>
#include <unistd.h>
#include <libnotify/notify.h>

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include <com/ubuntu/content/transfer.h>

#include <QDBusMetaType>
#include <QCache>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include <QUuid>

#include <cassert>

namespace cua = com::ubuntu::ApplicationManager;
namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

struct cucd::Service::RegHandler
{
    RegHandler(QString id, QString service, cuc::dbus::Handler* handler) : id(id),
        service(service),
        handler(handler)
    {
    }

    QString id;
    QString service;
    cuc::dbus::Handler* handler;
};

struct cucd::Service::Private : public QObject
{
    Private(QDBusConnection connection,
            const QSharedPointer<cucd::PeerRegistry>& registry,
            QSharedPointer<cua::ApplicationManager>& application_manager,
            QObject* parent)
            : QObject(parent),
              connection(connection),
              registry(registry),
              app_manager(application_manager)
    {
    }

    QDBusConnection connection;
    QSharedPointer<cucd::PeerRegistry> registry;
    QSet<cucd::Transfer*> active_transfers;
    QSet<RegHandler*> handlers;
    QSharedPointer<cua::ApplicationManager> app_manager;

};

cucd::Service::Service(QDBusConnection connection, const QSharedPointer<cucd::PeerRegistry>& peer_registry,
                       QSharedPointer<cua::ApplicationManager>& application_manager, QObject* parent)
        : QObject(parent),
          m_watcher(new QDBusServiceWatcher()),
          d(new Private{connection, peer_registry, application_manager, this})
{
    assert(!peer_registry.isNull());

    qDBusRegisterMetaType<cuc::Peer>();

    m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    m_watcher->setConnection(d->connection);
    QObject::connect(m_watcher, SIGNAL(serviceUnregistered(const QString&)),
            this,
            SLOT(handler_unregistered(const QString&)));
}

cucd::Service::~Service()
{
    TRACE() << Q_FUNC_INFO;
    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        TRACE() << Q_FUNC_INFO << "Destroying transfer:" << t->Id();
        delete t;
    }
}

void cucd::Service::Quit()
{
    QCoreApplication::instance()->quit();
}

QVariantList cucd::Service::KnownSourcesForType(const QString& type_id)
{
    QVariantList result;

    d->registry->enumerate_known_sources_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(QVariant::fromValue(peer));
        });

    return result;
}

QVariantList cucd::Service::KnownDestinationsForType(const QString& type_id)
{
    QVariantList result;

    d->registry->enumerate_known_destinations_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(QVariant::fromValue(peer));
        });

    return result;
}

QVariantList cucd::Service::KnownSharesForType(const QString& type_id)
{
    QVariantList result;

    d->registry->enumerate_known_shares_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(QVariant::fromValue(peer));
        });

    return result;
}

QDBusVariant cucd::Service::DefaultSourceForType(const QString& type_id)
{
    cuc::Peer peer = d->registry->default_source_for_type(Type(type_id));

    return QDBusVariant(QVariant::fromValue(peer));
}

QDBusObjectPath cucd::Service::CreateImportFromPeer(const QString& peer_id, const QString& app_id)
{
    TRACE() << Q_FUNC_INFO;
    QString dest_id = app_id;
    if (dest_id.isEmpty())
    {
        TRACE() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        dest_id = aa_profile(this->message().service());
    }
    return CreateTransfer(dest_id, peer_id, cuc::Transfer::Import);
}

bool cucd::Service::should_cancel (int st)
{
    TRACE() << Q_FUNC_INFO << "State:" << st;

    return (st != cuc::Transfer::finalized
            && st != cuc::Transfer::collected
            && st != cuc::Transfer::aborted);
}

void action_dismiss(NotifyNotification *notification, char *action, gpointer data)
{
    TRACE() << Q_FUNC_INFO;
    Q_UNUSED(notification);
    Q_UNUSED(action);
    Q_UNUSED(data);
}

void action_accept(NotifyNotification *notification, char *action, gpointer data)
{
    TRACE() << Q_FUNC_INFO;
    Q_UNUSED(notification);
    Q_UNUSED(action);

    cucd::Transfer* t = (cucd::Transfer*)data;
    t->Charge(QStringList());
}

void download_notify (cucd::Transfer* t)
{
    TRACE() << Q_FUNC_INFO;
    notify_init(t->source().toStdString().c_str());
    NotifyNotification* notification;

    notification = notify_notification_new (_("Download Complete"),
                                            "",
                                            "save");

    notify_notification_set_hint_string(notification,
                                        "x-canonical-snap-decisions",
                                        "true");


    notify_notification_set_hint_string(notification,
                                        "x-canonical-private-button-tint",
                                        "true");

    notify_notification_add_action (notification,
                                    "action_accept",
                                    _("Open"),
                                    action_accept,
                                    t,
                                    NULL);

    notify_notification_add_action (notification,
                                    "action_dismiss",
                                    _("Dismiss"),
                                    action_dismiss,
                                    t,
                                    NULL);

    GError *error = NULL;
    if (!notify_notification_show(notification, &error)) {
        qWarning() << "Failed to show snap decision:" << error->message;
        g_error_free (error);
    }

}

QDBusObjectPath cucd::Service::CreateExportToPeer(const QString& peer_id, const QString& app_id)
{
    TRACE() << Q_FUNC_INFO;
    QString src_id = app_id;
    if (src_id.isEmpty())
    {
        TRACE() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        src_id = aa_profile(this->message().service());
    }
    return CreateTransfer(peer_id, src_id, cuc::Transfer::Export);
}

QDBusObjectPath cucd::Service::CreateShareToPeer(const QString& peer_id, const QString& app_id)
{
    TRACE() << Q_FUNC_INFO;
    QString src_id = app_id;
    if (src_id.isEmpty())
    {
        TRACE() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        src_id = aa_profile(this->message().service());
    }
    return CreateTransfer(peer_id, src_id, cuc::Transfer::Share);
}

QDBusObjectPath cucd::Service::CreateTransfer(const QString& dest_id, const QString& src_id, int dir)
{
    TRACE() << Q_FUNC_INFO << "DEST:" << dest_id << "SRC:" << src_id << "DIRECTION:" << dir;

    static size_t import_counter{0}; import_counter++;

    QUuid uuid{QUuid::createUuid()};

    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        if (t->destination() == dest_id || t->source() == src_id)
        {
            TRACE() << Q_FUNC_INFO << "Found transfer for peer_id:" << src_id;
            if (should_cancel(t->State()))
            {
                TRACE() << Q_FUNC_INFO << "Aborting active transfer:" << t->Id();
                t->Abort();
            }
        }
    }

    auto transfer = new cucd::Transfer(import_counter, src_id, dest_id, dir, this);
    new TransferAdaptor(transfer);
    d->active_transfers.insert(transfer);

    auto destination = transfer->import_path();
    auto source = transfer->export_path();
    if (not d->connection.registerObject(source, transfer))
        TRACE() << "Problem registering object for path: " << source;
    d->connection.registerObject(destination, transfer);

    TRACE() << "Created transfer " << source << " -> " << destination;

    // Content flow is different for import
    if (dir == cuc::Transfer::Import)
    {
        connect(transfer, SIGNAL(StateChanged(int)), this, SLOT(handle_imports(int)));
        return QDBusObjectPath{destination};
    }

    connect(transfer, SIGNAL(StateChanged(int)), this, SLOT(handle_exports(int)));
    return QDBusObjectPath{source};
}

void cucd::Service::handle_imports(int state)
{
    TRACE() << Q_FUNC_INFO << state;
    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());
    TRACE() << Q_FUNC_INFO << "State: " << transfer->State() << "Id:" << transfer->Id();

    if (state == cuc::Transfer::initiated)
    {
        TRACE() << Q_FUNC_INFO << "initiated";
        if (d->app_manager->is_application_started(transfer->source().toStdString()))
            transfer->SetSourceStartedByContentHub(false);
        else
            transfer->SetSourceStartedByContentHub(true);

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            TRACE() << Q_FUNC_INFO << "ID:" << r->id << "Handler: " << r->service << "Transfer: " << transfer->source();
            if (r->id == transfer->source())
            {
                TRACE() << Q_FUNC_INFO << "Found handler for initiated transfer" << r->id;
                if (r->handler->isValid())
                    r->handler->HandleExport(QDBusObjectPath{transfer->export_path()});
                else
                    TRACE() << Q_FUNC_INFO << "Handler invalid";
            }
        }

        d->app_manager->invoke_application(transfer->source().toStdString());
    }

    if (state == cuc::Transfer::charged)
    {
        TRACE() << Q_FUNC_INFO << "Charged";
        if (transfer->WasSourceStartedByContentHub())
            d->app_manager->stop_application(transfer->source().toStdString());
        
        d->app_manager->invoke_application(transfer->destination().toStdString());

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            TRACE() << Q_FUNC_INFO << "ID:" << r->id << "Handler: " << r->service << "Transfer: " << transfer->destination();
            if (r->id == transfer->destination())
            {
                TRACE() << Q_FUNC_INFO << "Found handler for charged transfer" << r->id;
                if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{transfer->import_path()});
            }
        }
    }

    if (state == cuc::Transfer::aborted)
    {
        if (transfer->WasSourceStartedByContentHub())
        {
            bool shouldStop = true;
            Q_FOREACH (cucd::Transfer *t, d->active_transfers)
            {
                if (t->Id() != transfer->Id())
                {
                    if ((t->source() == transfer->source()) && (t->State() == cuc::Transfer::in_progress))
                    {
                        TRACE() << Q_FUNC_INFO << "Source has pending transfers:" << t->Id();
                        shouldStop = false;
                    }
                    if (t->destination() == transfer->destination())
                    {
                        TRACE() << Q_FUNC_INFO << "Destination has pending transfers:" << t->Id();
                        if (should_cancel(t->State()))
                            shouldStop = false;
                    }
                }
            }
            if (shouldStop)
                d->app_manager->stop_application(transfer->source().toStdString());            
        }
        d->app_manager->invoke_application(transfer->destination().toStdString());
    }
}

void cucd::Service::handle_exports(int state)
{
    TRACE() << Q_FUNC_INFO;
    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());

    TRACE() << Q_FUNC_INFO << "STATE:" << transfer->State();


    if (state == cuc::Transfer::initiated)
    {
        TRACE() << Q_FUNC_INFO << "Initiated";
        transfer->Handled();
    }

    if (state == cuc::Transfer::downloaded)
    {
        TRACE() << Q_FUNC_INFO << "Downloaded";
        download_notify(transfer);
    }

    if (state == cuc::Transfer::charged)
    {
        TRACE() << Q_FUNC_INFO << "Charged";
        if (d->app_manager->is_application_started(transfer->destination().toStdString()))
            transfer->SetSourceStartedByContentHub(false);
        else
            transfer->SetSourceStartedByContentHub(true);

        d->app_manager->invoke_application(transfer->destination().toStdString());

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            TRACE() << "Handler: " << r->service << "Transfer: " << transfer->destination();
            if (r->id == transfer->destination())
            {
                TRACE() << "Found handler for charged transfer" << r->id;
                if (transfer->Direction() == cuc::Transfer::Share && r->handler->isValid())
                    r->handler->HandleShare(QDBusObjectPath{transfer->import_path()});
                else if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{transfer->import_path()});
            }
        }
    }

    if (state == cuc::Transfer::finalized)
    {
        TRACE() << Q_FUNC_INFO << "Finalized";
        if (transfer->WasSourceStartedByContentHub())
            d->app_manager->stop_application(transfer->destination().toStdString());

        d->app_manager->invoke_application(transfer->source().toStdString());
    }

    if (state == cuc::Transfer::aborted)
    {
        TRACE() << Q_FUNC_INFO << "Aborted";
        if (transfer->WasSourceStartedByContentHub())
        {
            bool shouldStop = true;
            Q_FOREACH (cucd::Transfer *t, d->active_transfers)
            {
                if (t->Id() != transfer->Id())
                {
                    if ((t->source() == transfer->source()) && (t->State() == cuc::Transfer::in_progress))
                    {
                        TRACE() << Q_FUNC_INFO << "Source has pending transfers:" << t->Id();
                        shouldStop = false;
                    }
                    if (t->destination() == transfer->destination())
                    {
                        TRACE() << Q_FUNC_INFO << "Destination has pending transfers:" << t->Id();
                        if (should_cancel(t->State()))
                            shouldStop = false;
                    }
                }
            }
            if (shouldStop)
                d->app_manager->stop_application(transfer->destination().toStdString());
        }
        d->app_manager->invoke_application(transfer->source().toStdString());
    }
}

void cucd::Service::handler_unregistered(const QString& s)
{
    TRACE() << Q_FUNC_INFO << s;

    if (d->handlers.isEmpty())
        return;

    Q_FOREACH (RegHandler *r, d->handlers)
    {
        TRACE() << "Handler: " << r->id;
        if (r->service == s)
        {
            TRACE() << "Found match for " << r->id;
            d->handlers.remove(r);
            m_watcher->removeWatchedService(s);
            delete r;
        }
    }
}

void cucd::Service::RegisterImportExportHandler(const QString& peer_id, const QDBusObjectPath& handler)
{
    TRACE() << Q_FUNC_INFO << peer_id;
    bool exists = false;
    RegHandler* r;
    Q_FOREACH (RegHandler *rh, d->handlers)
    {
        TRACE() << "Handler: " << rh->id;
        if (rh->id == peer_id)
        {
            TRACE() << "Found existing handler for " << rh->id;
            exists = true;
            r = rh;
        }
    }

    if (!exists)
    {
        r = new RegHandler{peer_id,
            this->message().service(),
            new cuc::dbus::Handler(
                    this->message().service(),
                    handler.path(),
                    QDBusConnection::sessionBus(),
                    0)};
        d->handlers.insert(r);
        m_watcher->addWatchedService(r->service);
    }

    TRACE() << Q_FUNC_INFO << r->id;

    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        TRACE() << Q_FUNC_INFO << "SOURCE: " << t->source() << "DEST:" << t->destination() << "STATE:" << t->State();
        if ((t->source() == peer_id) && (t->State() == cuc::Transfer::initiated))
        {
            TRACE() << Q_FUNC_INFO << "Found source:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Import)
            {
                if (r->handler->isValid())
                    r->handler->HandleExport(QDBusObjectPath{t->export_path()});
            }
        }
        else if ((t->destination() == peer_id) && (t->State() == cuc::Transfer::charged))
        {
            TRACE() << Q_FUNC_INFO << "Found destination:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Export)
            {
                TRACE() << Q_FUNC_INFO << "Found import, calling HandleImport";
                if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{t->import_path()});
            } else if (t->Direction() == cuc::Transfer::Share)
            {
                TRACE() << Q_FUNC_INFO << "Found share, calling HandleShare";
                if (r->handler->isValid())
                    r->handler->HandleShare(QDBusObjectPath{t->import_path()});
            }
        }
        else if ((t->destination() == peer_id) && (t->State() == cuc::Transfer::downloaded))
        {
            TRACE() << Q_FUNC_INFO << "Found destination:" << peer_id << "Direction:" << t->Direction();
            if (t->Direction() == cuc::Transfer::Export)
            {
                TRACE() << Q_FUNC_INFO << "Found downloaded import, charging";
                if (r->handler->isValid())
                    t->Charge(QStringList());
            }
        }
    }
}

void cucd::Service::HandlerActive(const QString& peer_id)
{
    TRACE() << Q_FUNC_INFO << peer_id;
}
