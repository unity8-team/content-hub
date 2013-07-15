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

#include "service.h"

#include "peer_registry.h"

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>

#include <QCache>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>

#include <cassert>

namespace cucd = com::ubuntu::content::detail;

struct cucd::Service::Private : public QObject
{
    Private(const QSharedPointer<cucd::PeerRegistry>& registry, QObject* parent)
            : QObject(parent),
              registry(registry)
    {
    }

    QSharedPointer<cucd::PeerRegistry> registry;
};

cucd::Service::Service(const QSharedPointer<cucd::PeerRegistry>& peer_registry, QObject* parent)
        : QObject(parent),
          d(new Private{peer_registry, this})
{
    assert(!peer_registry.isNull());
}

cucd::Service::~Service() {}

void cucd::Service::Quit()
{
    QCoreApplication::instance()->quit();
}

QStringList cucd::Service::KnownPeersForType(const QString& type_id)
{
    QStringList result;
    
    d->registry->enumerate_known_peers_for_type(
        Type(type_id),
        [&result](const Peer& peer)
        {
            result.append(peer.id());
        });
    
    return result;
}

QString cucd::Service::DefaultPeerForType(const QString& type_id)
{
    auto peer = d->registry->default_peer_for_type(Type(type_id));

    return peer.id();
}
