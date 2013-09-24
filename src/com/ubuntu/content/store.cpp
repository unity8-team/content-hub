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

#include <com/ubuntu/content/store.h>

namespace cuc = com::ubuntu::content;

struct cuc::Store::Private
{
    QString uri;
};

cuc::Store::Store(const QString& uri, QObject* parent) : QObject(parent), d(new cuc::Store::Private{uri})
{
}

cuc::Store::Store(const cuc::Store& rhs) : QObject(rhs.parent()), d(rhs.d)
{
}

cuc::Store& cuc::Store::operator=(const cuc::Store& rhs)
{
    d = rhs.d;
    return *this;
}

cuc::Store::~Store()
{
}

const QString& cuc::Store::uri() const
{
    return d->uri;
}

