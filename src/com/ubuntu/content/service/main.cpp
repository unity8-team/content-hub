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
#include "common.h"
#include "registry.h"
#include "detail/service.h"
#include "detail/peer_registry.h"
#include "serviceadaptor.h"

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

namespace {
    void list(QSharedPointer<cucd::PeerRegistry> registry)
    {
        /* list known peers for pictures */
        QStringList result;
        registry->enumerate_known_peers_for_type(
            cuc::Type::Known::pictures(),
            [&result](const cuc::Peer& peer)
            {
                result.append(peer.id());
            });

        foreach (QString r, result) {
            qDebug() << "RESULT: " << r;
        }
    }


    void populate(QSharedPointer<cucd::PeerRegistry> registry)
    {
        registry->install_default_peer_for_type(cuc::Type::Known::pictures(), cuc::Peer("gallery-app"));
        registry->install_peer_for_type(cuc::Type::Known::pictures(), cuc::Peer("com.example.pictures"));
        list(registry);
    }
}

int main(int argc, char** argv)
{
    qDebug() << Q_FUNC_INFO << HUB_SERVICE_NAME;
    QCoreApplication *app = new QCoreApplication(argc, argv);

    auto connection = QDBusConnection::sessionBus();

    auto registry = QSharedPointer<cucd::PeerRegistry>(new Registry());

    auto server = new cucd::Service(connection, registry, app->parent());
    new ServiceAdaptor(server);

    connection.registerService(HUB_SERVICE_NAME);
    connection.registerObject(HUB_SERVICE_PATH, server, QDBusConnection::ExportAdaptors);

    /* Populate registry with dummy peers */
    populate(registry);

    return app->exec();
}



