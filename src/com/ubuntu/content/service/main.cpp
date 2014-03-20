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
#include <csignal>

#include "detail/app_manager.h"
#include "common.h"
#include "registry.h"
#include "detail/service.h"
#include "detail/peer_registry.h"
#include "serviceadaptor.h"

namespace cuca = com::ubuntu::ApplicationManager;
namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

namespace {
    void list(QSharedPointer<cucd::PeerRegistry> registry)
    {
        /* list known peers for pictures */
        QStringList result;
        registry->enumerate_known_sources_for_type(
            cuc::Type::Known::pictures(),
            [&result](const cuc::Peer& peer)
            {
                result.append(peer.id());
            });

        foreach (QString r, result) {
            qDebug() << "PEER: " << r;
        }
    }

    void shutdown(int sig)
    {
        qDebug() << Q_FUNC_INFO << sig;
        QCoreApplication::instance()->quit();
    }
}

int main(int argc, char** argv)
{
    int ret = 0;
    QCoreApplication *app = new QCoreApplication(argc, argv);

    auto connection = QDBusConnection::sessionBus();

    auto registry = QSharedPointer<cucd::PeerRegistry>(new Registry());

    auto app_manager = QSharedPointer<cuca::ApplicationManager>(new cucd::AppManager());

    auto server = new cucd::Service(connection, registry, app_manager, app->parent());
    new ServiceAdaptor(server);

    if (not connection.registerService(HUB_SERVICE_NAME))
    {
        qWarning() << "Failed to register" << HUB_SERVICE_NAME;
        ret = 1;
    }
    if (not connection.registerObject(HUB_SERVICE_PATH,
                                      server,
                                      QDBusConnection::ExportAdaptors))
    {
        qWarning() << "Failed to register object on" << HUB_SERVICE_PATH;
        ret = 1;
    }

    std::signal(SIGTERM, shutdown);
    std::signal(SIGHUP, shutdown);
    std::signal(SIGKILL, shutdown);
    std::signal(SIGINT, shutdown);

    if (ret == 1)
        app->exit(ret);
    else
    {
        /* list known peers */
        list(registry);
        ret = app->exec();
    }

    qDebug() << "Server exiting, cleaning up";
    delete server;
    return ret;
}
