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
            cua::ApplicationManager *application_manager,
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
    cua::ApplicationManager *app_manager;

    /* Removes the given transfer from the list of active transfer and does all the cleanup work */
    void clean_up_transfer(cucd::Transfer *transfer)
    {
        qDebug() << Q_FUNC_INFO << "Found aborted or collected transfer, removing";
        connection.unregisterObject(transfer->export_path());
        connection.unregisterObject(transfer->import_path());
        active_transfers.remove(transfer);
        qDebug() << Q_FUNC_INFO << "ACTIVE TRANSFERS:" << active_transfers.count();
    }

};

cucd::Service::Service(QDBusConnection connection, const QSharedPointer<cucd::PeerRegistry>& peer_registry,
                       cua::ApplicationManager *application_manager, QObject* parent)
        : QObject(parent),
          d(new Private{connection, peer_registry, application_manager, this})
{
    assert(!peer_registry.isNull());

    m_watcher = new QDBusServiceWatcher();
    m_watcher->setWatchMode(QDBusServiceWatcher::WatchForRegistration);
    m_watcher->setConnection(d->connection);
    QObject::connect(m_watcher, SIGNAL(serviceRegistered(const QString&)),
            this,
            SLOT(handler_registered(const QString&)));
}

cucd::Service::~Service() {}

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
                    HANDLER_PATH,
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
                    HANDLER_PATH,
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

void cucd::Service::connect_export_handler(const QString& peer_id, const QString& path, const QString& transfer)
{
    qDebug() << Q_FUNC_INFO;

    cuc::dbus::Handler *h = new cuc::dbus::Handler(
                handler_address(peer_id),
                path,
                QDBusConnection::sessionBus(),
                0);

    qDebug() << Q_FUNC_INFO << "h->isValid:" << h->isValid();
    if (h->isValid() && (not transfer.isEmpty()))
        h->HandleExport(QDBusObjectPath{transfer});
}

void cucd::Service::connect_import_handler(const QString& peer_id, const QString& path, const QString& transfer)
{
    qDebug() << Q_FUNC_INFO;

    cuc::dbus::Handler *h = new cuc::dbus::Handler(
                handler_address(peer_id),
                path,
                QDBusConnection::sessionBus(),
                0);

    qDebug() << Q_FUNC_INFO << "h->isValid:" << h->isValid();
    if (h->isValid() && (not transfer.isEmpty()))
        h->HandleImport(QDBusObjectPath{transfer});

}

QDBusObjectPath cucd::Service::CreateImportForTypeFromPeer(const QString& type_id, const QString& peer_id, const QString& app_id)
{
    static size_t import_counter{0}; import_counter++;

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
    this->connect_export_handler(peer_id, HANDLER_PATH, source);
    this->connect_import_handler(peer_id, HANDLER_PATH, destination);

    Q_UNUSED(type_id);

    d->app_manager->invoke_application(transfer->source().toStdString());

    return QDBusObjectPath{destination};
}

void cucd::Service::handle_transfer(int state)
{
    qDebug() << Q_FUNC_INFO;

    cucd::Transfer *transfer = static_cast<cucd::Transfer*>(sender());

    if (state == cuc::Transfer::aborted)
    {
//        d->app_manager->invoke_application(transfer->destination().toStdString());
        d->clean_up_transfer(transfer);
    }

    if (state == cuc::Transfer::collected)
    {
        d->clean_up_transfer(transfer);
    }

    if (state == cuc::Transfer::charged)
    {
        qDebug() << Q_FUNC_INFO << "Charged";
//        d->app_manager->invoke_application(transfer->destination().toStdString());
        this->connect_import_handler(transfer->destination(), HANDLER_PATH, transfer->import_path());
    }

    if (state == cuc::Transfer::initiated)
    {
        qDebug() << Q_FUNC_INFO << "Initiated";
        this->connect_export_handler(transfer->source(), HANDLER_PATH, transfer->export_path());
    }

    if (state == cuc::Transfer::in_progress)
    {
        qDebug() << Q_FUNC_INFO << "InProgress";
    }
}

void cucd::Service::RegisterImportExportHandler(const QString& /*type_id*/, const QString& peer_id, const QDBusObjectPath& handler)
{
    qDebug() << Q_FUNC_INFO << peer_id << ":" << handler.path();
}
