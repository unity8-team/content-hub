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
#include <QStringList>
#include "example.h"

namespace cuc = com::ubuntu::content;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Example *e = new Example();

    QString peerName;

    if (a.arguments().size() > 1)
        peerName = a.arguments().at(1);

    if (!peerName.isEmpty())
    {
        qDebug() << peerName;
        auto hub = cuc::Hub::Client::instance();

        auto peer = cuc::Peer{peerName};
        qDebug() << Q_FUNC_INFO << "PEER: " << peer.id();
        auto transfer = hub->create_import_from_peer(peer);
        transfer->start();
    }

    Q_UNUSED(e);

    return a.exec();
}
