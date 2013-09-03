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

#include <QCache>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include <QUuid>

#include <cassert>

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

struct cucd::Service::Private : public QObject
{
    Private(QDBusConnection connection,
            const QSharedPointer<cucd::PeerRegistry>& registry, 
            QObject* parent)
            : QObject(parent),
              connection(connection),
              registry(registry)
    {
    }

    QDBusConnection connection;
    QSharedPointer<cucd::PeerRegistry> registry;
    QSet<cucd::Transfer*> active_transfers;
    QMap<QString, QDBusObjectPath> registered_handlers;
};

cucd::Service::Service(QDBusConnection connection, const QSharedPointer<cucd::PeerRegistry>& peer_registry, QObject* parent)
        : QObject(parent),
          d(new Private{connection, peer_registry, this})
{
    assert(!peer_registry.isNull());
}

cucd::Service::~Service() {}

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

void cucd::Service::connect_export_handler(const QString& address, const QString& path, const QString& destination)
{
    qDebug() << Q_FUNC_INFO;

    cuc::dbus::Handler* h = new cuc::dbus::Handler(
                address,
                path,
                QDBusConnection::sessionBus(),
                0);

    h->HandleExport(QDBusObjectPath{destination});
}

QDBusObjectPath cucd::Service::CreateImportForTypeFromPeer(const QString& type_id, const QString& peer_id, const QString& app_id)
{
    static size_t import_counter{0}; import_counter++;

    static const QString importer_path_pattern{"/transfers/%1/import/%2"};
    static const QString exporter_path_pattern{"/transfers/%1/export/%2"};

    QUuid uuid{QUuid::createUuid()};

    QString destination = importer_path_pattern
            .arg(sanitize_path(app_id))
            .arg(import_counter);

    QString source = exporter_path_pattern
            .arg(sanitize_path(peer_id))
            .arg(import_counter);

    auto transfer = new cucd::Transfer(this);
    new TransferAdaptor(transfer);
    d->active_transfers.insert(transfer);

    if (not d->connection.registerObject(destination, transfer))
        qDebug() << "Problem registering object for path: " << destination;
    d->connection.registerObject(source, transfer);

    qDebug() << "Created transfer " << source << " -> " << destination;

    /* FIXME: Lookup export handler already registered and call handle_export
     * on it this needs to be replaced with something that listens for the
     * handler
     */

    /* iterate registered handlers */
    QMap<QString, QDBusObjectPath>::iterator i = d->registered_handlers.find(peer_id);

    if (i != d->registered_handlers.end())
    {
        QString address = handler_address(peer_id);
        QString handler_path = i.value().path();
        this->connect_export_handler(address, handler_path, destination);
    }
    /* end export handler hack */

    Q_UNUSED(type_id);

    return QDBusObjectPath{destination};
}

void cucd::Service::RegisterImportExportHandler(const QString& /*type_id*/, const QString& peer_id, const QDBusObjectPath& handler)
{
    qDebug() << Q_FUNC_INFO << peer_id << ":" << handler.path();
    d->registered_handlers.insert(peer_id, handler);
}
