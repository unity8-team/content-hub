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

    /* read environment variables */
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    if (environment.contains(QLatin1String("CONTENT_HUB_LOGGING_LEVEL"))) {
        bool isOk;
        int value = environment.value(
            QLatin1String("CONTENT_HUB_LOGGING_LEVEL")).toInt(&isOk);
        if (isOk)
            setLoggingLevel(value);
    }

    std::string handler = "export";
    QString url, text, appId;
    gchar* pkg = NULL;
    gchar* app = NULL;
    gchar* ver = NULL;

    /* URL handled looks like:
     * content:?pkg=foo&app=bar&ver=0.1&url=path&text=text
     * Only pkg is required.
     */

    QUrlQuery* query = new QUrlQuery(a.arguments().at(1).split("?").at(1));
    TRACE() << "Handling URL:" << query->query();

    if (query->hasQueryItem("pkg"))
        pkg = g_strdup(query->queryItemValue("pkg").toStdString().c_str());
    else {
        qWarning() << "PKG is required";
        return 1;
    }
    if (query->hasQueryItem("app"))
        app = g_strdup(query->queryItemValue("app").toStdString().c_str());
    if (query->hasQueryItem("ver"))
        ver = g_strdup(query->queryItemValue("ver").toStdString().c_str());
    if (query->hasQueryItem("handler"))
        handler = query->queryItemValue("handler").toStdString();
    url = query->queryItemValue("url");
    text = query->queryItemValue("text");
    TRACE() << "URL:" << url;
    TRACE() << "PKG:" << pkg;
    TRACE() << "APP:" << app;
    TRACE() << "VER:" << ver;
    TRACE() << "HANDLER:" << handler.c_str();


    appId = QString::fromLocal8Bit(ubuntu_app_launch_triplet_to_app_id(pkg, app, ver));
    if (appId.isNull())
        appId = QString(pkg);

    if (appId.isEmpty())
    {
        qWarning() << "Unable to determine peer";
        return 1;
    }

    AutoExporter exporter;
    if (!url.isEmpty())
        exporter.setUrl(url);

    if (!text.isEmpty())
        exporter.setText(text);

    TRACE() << "APP_ID:" << appId;

    auto hub = cuc::Hub::Client::instance();
    auto peer = cuc::Peer{appId};
    if (handler == "share") {
        auto transfer = hub->create_share_to_peer(peer);
        exporter.handle_export(transfer);
    } else {
        auto transfer = hub->create_export_to_peer(peer);
        exporter.handle_export(transfer);
    }

    return a.exec();
}
