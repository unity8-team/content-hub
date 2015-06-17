/*
 * Copyright (C) 2014 Canonical, Ltd.
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
#include <QUrl>
#include <sys/apparmor.h>

#include "autoexporter.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (qgetenv("APP_ID").isEmpty()) {
        qputenv("APP_ID", "content-hub-test-exporter");
    }

    QString peerName, url, profile;

    if (a.arguments().size() > 1)
        peerName = a.arguments().at(1);
    if (a.arguments().size() > 2)
        url = a.arguments().at(2);
    if (a.arguments().size() > 3)
        profile = a.arguments().at(3);

    if (not profile.isEmpty()) {
        int ret = 2;
        ret = aa_change_profile(profile.toStdString().c_str());
        if (ret != 0)
            return 1;
    }

    AutoExporter exporter;

    if (not url.isEmpty())
        exporter.setUrl(url);

    if (!peerName.isEmpty())
    {
        qDebug() << peerName;
        auto hub = cuc::Hub::Client::instance();

        auto peer = cuc::Peer{peerName};
        qDebug() << Q_FUNC_INFO << "PEER: " << peer.id();
        auto transfer = hub->create_export_to_peer(peer);
        exporter.handle_export(transfer);
    }

    return a.exec();
}
