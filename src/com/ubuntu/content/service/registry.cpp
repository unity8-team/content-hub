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

Registry::Registry() :
    m_defaultImportPeers(new QGSettings("com.ubuntu.content.hub.import.default",
                                  "/com/ubuntu/content/hub/peers/import/")),
    m_importPeers(new QGSettings("com.ubuntu.content.hub.import.all",
                           "/com/ubuntu/content/hub/peers/import/"))
{
}

Registry::~Registry() {}

cuc::Peer Registry::default_peer_for_type(cuc::Type type)
{
    qDebug() << Q_FUNC_INFO << type.id();
    if (m_defaultImportPeers->keys().contains(type.id()))
        return cuc::Peer(m_defaultImportPeers->get(type.id()).toString());
    else
        return cuc::Peer();
}

void Registry::enumerate_known_peers_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << Q_FUNC_INFO << type.id();

    Q_FOREACH (QString k, m_importPeers->get(type.id()).toStringList())
    {
        qDebug() << Q_FUNC_INFO << k;
        for_each(k);
    }
}

void Registry::enumerate_known_peers(const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << Q_FUNC_INFO;

    Q_FOREACH (QString type_id, m_importPeers->keys())
    {
        qDebug() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_importPeers->get(type_id).toStringList())
        {
            qDebug() << Q_FUNC_INFO << k;
            for_each(k);
        }
    }
}

bool Registry::install_default_peer_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    if (m_defaultImportPeers->keys().contains(type.id()))
    {
        qDebug() << Q_FUNC_INFO << "Default peer for" << type.id() << "already installed.";
        return false;
    }

    this->install_peer_for_type(type, peer);
    return m_defaultImportPeers->trySet(type.id(), QVariant(peer.id()));
}

bool Registry::install_peer_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    QStringList l = m_importPeers->get(type.id()).toStringList();
    if (not l.contains(peer.id()))
    {
        l.append(peer.id());
        return m_importPeers->trySet(type.id(), QVariant(l));
    }
    return false;
}

bool Registry::remove_peer(cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "peer:" << peer.id();
    bool ret = false;
    Q_FOREACH (QString type_id, m_importPeers->keys())
    {
        QStringList l = m_importPeers->get(type_id).toStringList();
        if (l.contains(peer.id()))
        {
            l.removeAll(peer.id());
            ret = m_importPeers->trySet(type_id, QVariant(l));
        }
    }
    return ret;
}
