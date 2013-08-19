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

#include "registry.h"
#include <QDebug>

Registry::Registry()
{
    m_defaultPeers = new QMap<cuc::Type, cuc::Peer>();
    m_peers = new QMap<cuc::Type, cuc::Peer>();
}

cuc::Peer Registry::default_peer_for_type(cuc::Type type)
{
    qDebug() << "default_peer_for_type" << type.id();
    if (m_defaultPeers->contains(type))
        return m_defaultPeers->find(type).value();
    else
        return cuc::Peer();
}


void Registry::enumerate_known_peers_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << "enumerate_known_peers_for_type:" << type.id();
    for(QMap<cuc::Type,cuc::Peer>::iterator i = m_peers->begin(); i != m_peers->end(); )
    {
        if(i.key() == type)
        {
            for_each(i.value());
        }
        i++;
    }
}

bool Registry::install_default_peer_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << "install_default_peer_for_type, type:" << type.id() << "peer:" << peer.id();
    if (m_defaultPeers->contains(type))
    {
        qDebug() << "Default peer for" << type.id() << "already installed.";
        return false;
    }
    m_defaultPeers->insert(type, peer);
    this->install_peer_for_type(type, peer);
    return true;
}

bool Registry::install_peer_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << "install_peer_for_type, type:" << type.id() << "peer:" << peer.id();
    m_peers->insertMulti(type, peer);
    return true;
}
