/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */
#ifndef PEER_REGISTRY_H_
#define PEER_REGISTRY_H_

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>

#include <functional>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{
class PeerRegistry
{
  public:
    PeerRegistry() = default;
    PeerRegistry(const PeerRegistry& registry) = delete;
    virtual ~PeerRegistry() = default;

    PeerRegistry& operator=(const PeerRegistry&) = delete;

    virtual Peer default_source_for_type(Type) = 0;
    virtual void enumerate_known_peers(const std::function<void(const Peer&)>& for_each) = 0;
    virtual void enumerate_known_sources_for_type(Type, const std::function<void(const Peer&)>& for_each) = 0;
    virtual void enumerate_known_destinations_for_type(Type, const std::function<void(const Peer&)>& for_each) = 0;
    virtual void enumerate_known_shares_for_type(Type, const std::function<void(const Peer&)>& for_each) = 0;
    virtual bool install_default_source_for_type(Type, Peer) = 0;
    virtual bool install_source_for_type(Type, Peer) = 0;
    virtual bool install_destination_for_type(Type, Peer) = 0;
    virtual bool install_share_for_type(Type, Peer) = 0;
    virtual bool remove_peer(Peer peer) = 0;


};
}
}
}
}

#endif // PEER_REGISTRY_H_
