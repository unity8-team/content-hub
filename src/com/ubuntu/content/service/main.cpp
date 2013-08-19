/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include <QCoreApplication>
#include <QDebug>

#include "serviceregistry.h"
#include "detail/service.h"
#include "detail/peer_registry.h"
#include "serviceadaptor.h"

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

int main(int argc, char** argv)
{
    QCoreApplication *app = new QCoreApplication(argc, argv);
    const QString name = "com.ubuntu.content.dbus.Service";
    const QString path = "/";

    auto connection = QDBusConnection::sessionBus();

    auto registry = QSharedPointer<cucd::PeerRegistry>(new ServiceRegistry());
    auto peer = cuc::Peer("com.example.pictures");

    auto server = new cucd::Service(connection, registry, app->parent());
    new ServiceAdaptor(server);

    connection.registerService(name);
    connection.registerObject(path, server, QDBusConnection::ExportAdaptors);

    registry->install_default_peer_for_type(cuc::Type::Known::pictures(), peer);
    return app->exec();
}



