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

#ifndef REGISTRY_H
#define REGISTRY_H

#include <QGSettings/QGSettings>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include "detail/peer_registry.h"

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

class Registry : public cucd::PeerRegistry
{

public:
    Registry();
    ~Registry();
    cuc::Peer default_source_for_type(cuc::Type type);
    void enumerate_known_peers(const std::function<void(const cuc::Peer&)>& for_each);
    void enumerate_known_sources_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>& for_each);   
    void enumerate_known_destinations_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>& for_each);
    void enumerate_known_shares_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>& for_each);
    bool install_default_source_for_type(cuc::Type type, cuc::Peer peer);
    bool install_source_for_type(cuc::Type type, cuc::Peer peer);    
    bool install_destination_for_type(cuc::Type type, cuc::Peer peer);
    bool install_share_for_type(cuc::Type type, cuc::Peer peer);
    bool remove_peer(cuc::Peer peer);
    bool peer_is_legacy(QString type);

private:
    QScopedPointer<QGSettings> m_defaultSources;
    QScopedPointer<QGSettings> m_sources;
    QScopedPointer<QGSettings> m_dests;
    QScopedPointer<QGSettings> m_shares;
};

#endif // REGISTRY_H
