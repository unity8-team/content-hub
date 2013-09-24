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

#include "handler.h"
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
    QSharedPointer<cua::ApplicationManager> app_manager;
};

cucd::Service::Service(QDBusConnection connection, const QSharedPointer<cucd::PeerRegistry>& peer_registry,
                       QSharedPointer<cua::ApplicationManager>& application_manager, QObject* parent)
        : QObject(parent),
          m_watcher(new QDBusServiceWatcher()),
          d(new Private{connection, peer_registry, application_manager, this})
{
    assert(!peer_registry.isNull());

    m_watcher->setWatchMode(QDBusServiceWatcher::WatchForRegistration);
    m_watcher->setConnection(d->connection);
    QObject::connect(m_watcher, SIGNAL(serviceRegistered(const QString&)),
            this,
            SLOT(handler_registered(const QString&)));
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

void cucd::Service::handler_registered(const QString& name)
{
    qDebug() << Q_FUNC_INFO << name;
    Q_FOREACH (cucd::Transfer *t, d->active_transfers)
    {
        if (handler_address(t->source()) == name)
        {
            qDebug() << Q_FUNC_INFO << "Found source:" << name;
            cuc::dbus::Handler *h = new cuc::dbus::Handler(
                    name,
                    handler_path(t->source()),
                    QDBusConnection::sessionBus(),
                    0);
            if (h->isValid())
                h->HandleExport(QDBusObjectPath{t->export_path()});
        }
        else if (handler_address(t->destination()) == name)
        {
            qDebug() << Q_FUNC_INFO << "Found destination:" << name;
            cuc::dbus::Handler *h = new cuc::dbus::Handler(
                    name,
                    handler_path(t->destination()),
                    QDBusConnection::sessionBus(),
                    0);
            if (h->isValid())
                h->HandleImport(QDBusObjectPath{t->import_path()});
        }
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

void cucd::Service::connect_export_handler(const QString& peer_id, const QString& transfer)
{
    qDebug() << Q_FUNC_INFO;

    cuc::dbus::Handler *h = new cuc::dbus::Handler(
                handler_address(peer_id),
                handler_path(peer_id),
                QDBusConnection::sessionBus(),
                0);

    qDebug() << Q_FUNC_INFO << "h->isValid:" << h->isValid();
    if (h->isValid() && (not transfer.isEmpty()))
        h->HandleExport(QDBusObjectPath{transfer});
}

void cucd::Service::connect_import_handler(const QString& peer_id, const QString& transfer)
{
    qDebug() << Q_FUNC_INFO;

    cuc::dbus::Handler *h = new cuc::dbus::Handler(
                handler_address(peer_id),
                handler_path(peer_id),
                QDBusConnection::sessionBus(),
                0);

    qDebug() << Q_FUNC_INFO << "h->isValid:" << h->isValid();
    if (h->isValid() && (not transfer.isEmpty()))
        h->HandleImport(QDBusObjectPath{transfer});
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

    /* watch for handlers */
    m_watcher->addWatchedService(handler_address(peer_id));
    qDebug() << Q_FUNC_INFO << "Watches:" << m_watcher->watchedServices();
    this->connect_export_handler(peer_id, source);
    this->connect_import_handler(app_id, destination);

    Q_UNUSED(type_id);

    d->app_manager->invoke_application(transfer->source().toStdString());

    return QDBusObjectPath{destination};
}

void cucd::Service::handle_transfer(int state)
{
    qDebug() << Q_FUNC_INFO;
    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());

    if (state == cuc::Transfer::charged)
    {
        qDebug() << Q_FUNC_INFO << "Charged";
        this->connect_import_handler(transfer->destination(), transfer->import_path());
    }

    if (state == cuc::Transfer::initiated)
    {
        qDebug() << Q_FUNC_INFO << "Initiated";
        this->connect_export_handler(transfer->source(), transfer->export_path());
    }
}

void cucd::Service::RegisterImportExportHandler(const QString& /*type_id*/, const QString& peer_id, const QDBusObjectPath& handler)
{
    qDebug() << Q_FUNC_INFO << peer_id << ":" << handler.path();
}
