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

#include "debug.h"
#include "registry.h"
#include "utils.cpp"
#include <upstart-app-launch.h>

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
        if (m_defaultSources->keys().contains(type.id()))
        {
            QString peer_id = m_defaultSources->get(type.id()).toString();
            QStringList as(m_defaultSources->get(type.id()).toStringList());
            if (!as.isEmpty())
            {
                std::string pkg = as[0].toStdString();
                std::string app = as[1].toStdString();
                std::string ver = as[2].toStdString();
                cuc::Peer peer(QString::fromLocal8Bit(upstart_app_launch_triplet_to_app_id(pkg.c_str(), app.c_str(), ver.c_str())));
                install_source_for_type(type, cuc::Peer{peer.id(), true});
            }
        }
    }
}

Registry::~Registry() {}

cuc::Peer Registry::default_source_for_type(cuc::Type type)
{
    TRACE() << Q_FUNC_INFO << type.id();
    if (m_defaultSources->keys().contains(type.id()))
    {
        QStringList as(m_defaultSources->get(type.id()).toStringList());
        if (!as.isEmpty())
        {
            std::string pkg = as[0].toStdString();
            std::string app = as[1].toStdString();
            std::string ver = as[2].toStdString();
            return cuc::Peer(QString::fromLocal8Bit(upstart_app_launch_triplet_to_app_id(pkg.c_str(), app.c_str(), ver.c_str())), true);
        }
    }

    return cuc::Peer();
}

void Registry::enumerate_known_peers(const std::function<void(const cuc::Peer&)>&for_each)
{
    TRACE() << Q_FUNC_INFO;

    Q_FOREACH (QString type_id, m_sources->keys())
    {
        TRACE() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_sources->get(type_id).toStringList())
        {
            TRACE() << Q_FUNC_INFO << k;
            for_each(cuc::Peer{k});
        }
    }
    Q_FOREACH (QString type_id, m_dests->keys())
    {
        TRACE() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_dests->get(type_id).toStringList())
        {
            TRACE() << Q_FUNC_INFO << k;
            for_each(cuc::Peer{k});
        }
    }
    Q_FOREACH (QString type_id, m_shares->keys())
    {
        TRACE() << Q_FUNC_INFO << type_id;
        Q_FOREACH (QString k, m_shares->get(type_id).toStringList())
        {
            TRACE() << Q_FUNC_INFO << k;
            for_each(cuc::Peer{k});
        }
    }
}

void Registry::enumerate_known_sources_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    TRACE() << Q_FUNC_INFO << type.id();

    if (type == cuc::Type::unknown())
        return;

    Q_FOREACH (QString k, m_sources->get(type.id()).toStringList())
    {
        TRACE() << Q_FUNC_INFO << k;
        bool defaultPeer = false;
        QStringList as(m_defaultSources->get(type.id()).toStringList());
        if (!as.isEmpty())
        {   
            std::string pkg = as[0].toStdString();
            std::string app = as[1].toStdString();
            std::string ver = as[2].toStdString();
            defaultPeer = QString::fromLocal8Bit(upstart_app_launch_triplet_to_app_id(pkg.c_str(), app.c_str(), ver.c_str())) == k;
        }
        for_each(cuc::Peer{k, defaultPeer});
    }
}

void Registry::enumerate_known_destinations_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    TRACE() << Q_FUNC_INFO << type.id();
    Q_FOREACH (QString k, m_dests->get(type.id()).toStringList())
    {
        TRACE() << Q_FUNC_INFO << k;
        for_each(cuc::Peer{k});
    }
}

void Registry::enumerate_known_shares_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>&for_each)
{
    TRACE() << Q_FUNC_INFO << type.id();

    Q_FOREACH (QString k, m_shares->get(type.id()).toStringList())
    {
        TRACE() << Q_FUNC_INFO << k;
        for_each(cuc::Peer{k});
    }
}

bool Registry::install_default_source_for_type(cuc::Type type, cuc::Peer peer)
{
    TRACE() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
    if (m_defaultSources->keys().contains(type.id()))
    {
        TRACE() << Q_FUNC_INFO << "Default peer for" << type.id() << "already installed.";
        return false;
    }

    this->install_source_for_type(type, peer);
    return m_defaultSources->trySet(type.id(), QVariant(peer.id()));
}

bool Registry::install_source_for_type(cuc::Type type, cuc::Peer peer)
{
    TRACE() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
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
    TRACE() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
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
    TRACE() << Q_FUNC_INFO << "type:" << type.id() << "peer:" << peer.id();
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
    TRACE() << Q_FUNC_INFO << "peer:" << peer.id();
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
