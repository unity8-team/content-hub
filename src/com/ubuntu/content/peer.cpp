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

#include <com/ubuntu/content/peer.h>

namespace cuc = com::ubuntu::content;

struct cuc::Peer::Private
{
    QString id;
};

const cuc::Peer& cuc::Peer::unknown()
{
    static cuc::Peer peer;
    return peer;
}

cuc::Peer::Peer(const QString& id, QObject* parent) : QObject(parent), d(new cuc::Peer::Private{id})
{
}

cuc::Peer::Peer(const cuc::Peer& rhs) : QObject(rhs.parent()), d(rhs.d)
{
}

cuc::Peer::~Peer()
{
}

cuc::Peer& cuc::Peer::operator=(const cuc::Peer& rhs)
{
    d = rhs.d;    
    return *this;
}

bool cuc::Peer::operator==(const cuc::Peer& rhs) const
{
    if (d == rhs.d)
        return true;

    return d->id == rhs.d->id;
}

const QString& cuc::Peer::id() const
{
    return d->id;
}
