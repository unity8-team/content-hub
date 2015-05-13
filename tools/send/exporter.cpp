/*
 * Copyright (C) 2015 Canonical, Ltd.
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
#include <QStringList>
#include <QUrlQuery>
#include <ubuntu-app-launch.h>

#include "autoexporter.h"
#include "debug.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (qgetenv("APP_ID").isEmpty()) {
        qputenv("APP_ID", "content-hub-send-file");
    }

    std::string pkg, app;
    std::string handler = "export";
    std::string ver = "current-user-version";
    QString url, peerName;

    /* URL handled looks like:
     * content:?pkg=foo&app=bar&ver=0.1&url=path
     * pkg and app are required, if ver isn't included we use the appid 
     * wildcard of current-user-version
     */

    QUrlQuery* query = new QUrlQuery(a.arguments().at(1).split("?").at(1));
    TRACE() << "Handling URL:" << query->query();

    if (query->hasQueryItem("pkg"))
        pkg = query->queryItemValue("pkg").toStdString();
    if (query->hasQueryItem("app"))
        app = query->queryItemValue("app").toStdString();
    if (query->hasQueryItem("ver"))
        ver = query->queryItemValue("ver").toStdString();
    if (query->hasQueryItem("handler"))
        handler = query->queryItemValue("handler").toStdString();
    url = query->queryItemValue("url");
    TRACE() << "URL:" << url;
    TRACE() << "PKG:" << pkg.c_str();
    TRACE() << "APP:" << app.c_str();
    TRACE() << "VER:" << ver.c_str();
    TRACE() << "HANDLER:" << handler.c_str();
    peerName = QString::fromLocal8Bit(ubuntu_app_launch_triplet_to_app_id(pkg.c_str(), app.c_str(), ver.c_str()));

    if (url.isEmpty())
    {
        qWarning() << "URL is required";
        return 1;
    }

    AutoExporter exporter;
    exporter.setUrl(url);

    if (peerName.isEmpty())
    {
        qWarning() << "Unable to determine peer";
        return 1;
    }

    TRACE() << "PEER:" << peerName;

    auto hub = cuc::Hub::Client::instance();
    auto peer = cuc::Peer{peerName};
    if (handler == "share") {
        auto transfer = hub->create_share_to_peer(peer);
        exporter.handle_export(transfer);
    } else {
        auto transfer = hub->create_export_to_peer(peer);
        exporter.handle_export(transfer);
    }

    return a.exec();
}
