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

#include <QObject>
#include <QtCore>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include "detail/peer_registry.h"

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

class Registry : public cucd::PeerRegistry
{

public:
    Registry();
    cuc::Peer default_peer_for_type(cuc::Type type);
    void enumerate_known_peers_for_type(cuc::Type type, const std::function<void(const cuc::Peer&)>& for_each);
    bool install_default_peer_for_type(cuc::Type type, cuc::Peer peer);
    bool install_peer_for_type(cuc::Type type, cuc::Peer peer);

signals:
    
public slots:

private:
    QMap<cuc::Type,cuc::Peer> *m_defaultPeers;
    QMap<cuc::Type,cuc::Peer> *m_peers;
};

#endif // REGISTRY_H
