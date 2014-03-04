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
#include "utils.cpp"

Registry::Registry() :
    m_defaultSources(new QGSettings("com.ubuntu.content.hub.default",
                                  "/com/ubuntu/content/hub/peers/")),
    m_sources(new QGSettings("com.ubuntu.content.hub.source",
                           "/com/ubuntu/content/hub/source/")),
    m_dests(new QGSettings("com.ubuntu.content.hub.destination",
                           "/com/ubuntu/content/hub/destination/")),
    m_shares(new QGSettings("com.ubuntu.content.hub.share",
                           "/com/ubuntu/content/hub/share/"))
{
    /* ensure all default sources are registered as available sources */
    QList<cuc::Type> types = known_types();
    Q_FOREACH (cuc::Type type, types)
    {
        QString peer_id = m_defaultSources->get(type.id()).toString();
        install_source_for_type(type, cuc::Peer{peer_id});
    }
}

Registry::~Registry() {}

cuc::Peer Registry::default_source_for_type(cuc::Type type)
{
    qDebug() << Q_FUNC_INFO << type.id();
    if (m_defaultSources->keys().contains(type.id()))
        return cuc::Peer(m_defaultSources->get(type.id()).toString());
    else
        return cuc::Peer();
}

void Registry::enumerate_known_peers(const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << Q_FUNC_INFO;

    Q_FOREACH (QString type_id, m_sources->keys())
    {
        qDebug() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_sources->get(type_id).toStringList())
        {
            qDebug() << Q_FUNC_INFO << k;
            for_each(k);
        }
    }
    Q_FOREACH (QString type_id, m_dests->keys())
    {
        qDebug() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_dests->get(type_id).toStringList())
        {
            qDebug() << Q_FUNC_INFO << k;
            for_each(k);
        }
    }
    Q_FOREACH (QString type_id, m_shares->keys())
    {
        qDebug() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_shares->get(type_id).toStringList())
        {
            qDebug() << Q_FUNC_INFO << k;
            for_each(k);
        }
    }
}

void Registry::enumerate_known_sources_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << Q_FUNC_INFO << type.id();

    Q_FOREACH (QString k, m_sources->get(type.id()).toStringList())
    {
        qDebug() << Q_FUNC_INFO << k;
        for_each(k);
    }
}

void Registry::enumerate_known_destinations_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << Q_FUNC_INFO << type.id();
    Q_FOREACH (QString k, m_dests->get(type.id()).toStringList())
    {
        qDebug() << Q_FUNC_INFO << k;
        for_each(k);
    }
}

void Registry::enumerate_known_shares_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    qDebug() << Q_FUNC_INFO << type.id();

    Q_FOREACH (QString k, m_shares->get(type.id()).toStringList())
    {
        qDebug() << Q_FUNC_INFO << k;
        for_each(k);
    }
}

bool Registry::install_default_source_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    if (m_defaultSources->keys().contains(type.id()))
    {
        qDebug() << Q_FUNC_INFO << "Default peer for" << type.id() << "already installed.";
        return false;
    }

    this->install_source_for_type(type, peer);
    return m_defaultSources->trySet(type.id(), QVariant(peer.id()));
}

bool Registry::install_source_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    QStringList l = m_sources->get(type.id()).toStringList();
    if (not l.contains(peer.id()))
    {
        l.append(peer.id());
        return m_sources->trySet(type.id(), QVariant(l));
    }
    return false;
}

bool Registry::install_destination_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    QStringList l = m_dests->get(type.id()).toStringList();
    if (not l.contains(peer.id()))
    {
        l.append(peer.id());
        return m_dests->trySet(type.id(), QVariant(l));
    }
    return false;
}

bool Registry::install_share_for_type(cuc::Type type, cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    QStringList l = m_shares->get(type.id()).toStringList();
    if (not l.contains(peer.id()))
    {
        l.append(peer.id());
        return m_shares->trySet(type.id(), QVariant(l));
    }
    return false;
}

bool Registry::remove_peer(cuc::Peer peer)
{
    qDebug() << Q_FUNC_INFO << "peer:" << peer.id();
    bool ret = false;
    Q_FOREACH (QString type_id, m_sources->keys())
    {
        QStringList l = m_sources->get(type_id).toStringList();
        if (l.contains(peer.id()))
        {
            l.removeAll(peer.id());
            ret = m_sources->trySet(type_id, QVariant(l));
        }
    }
    Q_FOREACH (QString type_id, m_dests->keys())
    {
        QStringList l = m_dests->get(type_id).toStringList();
        if (l.contains(peer.id()))
        {
            l.removeAll(peer.id());
            ret = m_dests->trySet(type_id, QVariant(l));
        }
    }
    Q_FOREACH (QString type_id, m_shares->keys())
    {
        QStringList l = m_shares->get(type_id).toStringList();
        if (l.contains(peer.id()))
        {
            l.removeAll(peer.id());
            ret = m_shares->trySet(type_id, QVariant(l));
        }
    }
    return ret;
}
