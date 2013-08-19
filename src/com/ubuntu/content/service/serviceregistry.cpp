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

#include "serviceregistry.h"
#include <QDebug>

cuc::Peer ServiceRegistry::default_peer_for_type(cuc::Type)
{
    qDebug() << "default_peer_for_type";
    const cuc::Peer peer = cuc::Peer("com.example.pictures");
    return peer;
}


void ServiceRegistry::enumerate_known_peers_for_type(cuc::Type, const std::function<void(const cuc::Peer&)>&)
{
    qDebug() << "enumerate_known_peers_for_type";
}

bool ServiceRegistry::install_default_peer_for_type(cuc::Type, cuc::Peer)
{
    qDebug() << "install_default_peer_for_type";
    return true;
}

bool ServiceRegistry::install_peer_for_type(cuc::Type, cuc::Peer)
{
    qDebug() << "install_peer_for_type";
    return true;
}
