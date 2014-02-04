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

#include "service.h"

#include "peer_registry.h"
#include "transfer.h"
#include "transferadaptor.h"
#include "utils.cpp"
#include "ContentHandlerInterface.h"

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include <com/ubuntu/content/transfer.h>

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
    RegHandler(QString id, QString service, QString instance, cuc::dbus::Handler* handler) : id(id),
        service(service),
        instance(instance),
        handler(handler)
    {

    }

    QString id;
    QString service;
    QString instance;
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

    m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    m_watcher->setConnection(d->connection);
    QObject::connect(m_watcher, SIGNAL(serviceUnregistered(const QString&)),
            this,
            SLOT(handler_unregistered(const QString&)));
}

cucd::Service::~Service()
{
    qDebug() << Q_FUNC_INFO;
    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        qDebug() << Q_FUNC_INFO << "Destroying transfer:" << t->Id();
        delete t;
    }
}

void cucd::Service::Quit()
{
    QCoreApplication::instance()->quit();
}

QStringList cucd::Service::KnownPeersForType(const QString& type_id)
{
    QStringList result;

    d->registry->enumerate_known_peers_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(peer.id());
        });

    return result;
}

QString cucd::Service::DefaultPeerForType(const QString& type_id)
{
    auto peer = d->registry->default_peer_for_type(Type(type_id));

    return peer.id();
}

QDBusObjectPath cucd::Service::CreateImportForTypeFromPeer(const QString& type_id, const QString& peer_id, const QString& dest_id)
{
    qDebug() << Q_FUNC_INFO;

    static size_t import_counter{0}; import_counter++;

    QString app_id = dest_id;
    if (app_id.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "APP_ID isnt' set, attempting to get it from AppArmor";
        app_id = aa_profile(this->message().service());
    }

    qDebug() << Q_FUNC_INFO << "APP_ID:" << app_id;

    QUuid uuid{QUuid::createUuid()};

    auto transfer = new cucd::Transfer(import_counter, peer_id, app_id, this);
    new TransferAdaptor(transfer);
    d->active_transfers.insert(transfer);

    auto destination = transfer->import_path();
    auto source = transfer->export_path();
    if (not d->connection.registerObject(destination, transfer))
        qDebug() << "Problem registering object for path: " << destination;
    d->connection.registerObject(source, transfer);

    qDebug() << "Created transfer " << source << " -> " << destination;

    connect(transfer, SIGNAL(StateChanged(int)), this, SLOT(handle_transfer(int)));

    Q_UNUSED(type_id);

    return QDBusObjectPath{destination};
}

void cucd::Service::handle_transfer(int state)
{
    qDebug() << Q_FUNC_INFO;
    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());

    if (state == cuc::Transfer::initiated)
    {
        qDebug() << Q_FUNC_INFO << "Initiated";
        if (d->app_manager->is_application_started(transfer->source().toStdString()))
            transfer->SetSourceStartedByContentHub(false);
        else
            transfer->SetSourceStartedByContentHub(true);

        /*
        std::string instance_id = d->app_manager->start_application(
                    transfer->source().toStdString(),
                    transfer->export_path().toStdString());
        transfer->SetInstanceId(QString::fromStdString(instance_id));
        */
        d->app_manager->invoke_application(
                    transfer->source().toStdString(),
                    QString::number(transfer->Id()).toStdString());
    }

    if (state == cuc::Transfer::charged)
    {
        qDebug() << Q_FUNC_INFO << "Charged";
        if (transfer->WasSourceStartedByContentHub())
            d->app_manager->stop_application(transfer->source().toStdString(), transfer->InstanceId().toStdString());
        d->app_manager->invoke_application(
                    transfer->destination().toStdString(),
                    QString::number(transfer->Id()).toStdString());

        Q_FOREACH (RegHandler *r, d->handlers)
        {
            qDebug() << "Handler: " << r->service << "Transfer: " << transfer->destination();
            if (r->id == transfer->destination())
            {
                qDebug() << "Found handler for charged transfer" << r->id;
                if (r->handler->isValid())
                    r->handler->HandleImport(QDBusObjectPath{transfer->import_path()});
            }
        }
    }

    if (state == cuc::Transfer::aborted)
    {
        if (transfer->WasSourceStartedByContentHub())
            d->app_manager->stop_application(transfer->source().toStdString(), transfer->InstanceId().toStdString());

        d->app_manager->invoke_application(
                    transfer->destination().toStdString(),
                    QString::number(transfer->Id()).toStdString());
    }
}

void cucd::Service::handler_unregistered(const QString& s)
{
    qDebug() << Q_FUNC_INFO << s;

    if (d->handlers.isEmpty())
        return;

    Q_FOREACH (RegHandler *r, d->handlers)
    {
        qDebug() << "Handler: " << r->id;
        if (r->service == s)
        {
            qDebug() << "Found match for " << r->id;
            d->handlers.remove(r);
            m_watcher->removeWatchedService(s);
            delete r;
        }
    }
}

void cucd::Service::RegisterImportExportHandler(const QString& instance_id, const QString& peer_id, const QDBusObjectPath& handler)
{
    RegHandler* r = new RegHandler{peer_id,
            this->message().service(),
            instance_id,
            new cuc::dbus::Handler(
                    this->message().service(),
                    handler.path(),
                    QDBusConnection::sessionBus(),
                    0)};
    d->handlers.insert(r);
    m_watcher->addWatchedService(r->service);

    qDebug() << Q_FUNC_INFO << r->id;

    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        qDebug() << Q_FUNC_INFO << "SOURCE: " << t->source() << "STATE:" << t->State();
        // FIXME: Don't check instance_id because we can't handle multiple instances yet
        //if ((t->source() == peer_id) && (t->InstanceId() == instance_id))
        if ((t->source() == peer_id) && (t->State() == cuc::Transfer::initiated))
        {
            qDebug() << Q_FUNC_INFO << "Found source:" << peer_id;
            if (r->handler->isValid())
                r->handler->HandleExport(QDBusObjectPath{t->export_path()});
        }
        else if ((t->destination() == peer_id) && (t->State() == cuc::Transfer::charged))
        {
            qDebug() << Q_FUNC_INFO << "Found destination:" << peer_id;
            if (r->handler->isValid())
                r->handler->HandleImport(QDBusObjectPath{t->import_path()});
        }
    }
}
