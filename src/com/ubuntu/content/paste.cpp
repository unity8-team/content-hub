/*
 * Copyright © 2016 Canonical Ltd.
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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include <com/ubuntu/content/paste.h>

#include "paste_p.h"
#include "utils.cpp"

namespace cuc = com::ubuntu::content;

cuc::Paste::Paste(const QSharedPointer<cuc::Paste::Private>& d, QObject* parent)
        : QObject(parent),
          d(d)
{
}

cuc::Paste::~Paste()
{
    TRACE() << Q_FUNC_INFO;
}

int cuc::Paste::id() const
{
    return d->id();
}

QMimeData* cuc::Paste::mimeData()
{
    return deserializeMimeData(d->mimeData());
}

QString cuc::Paste::source() const
{
    return d->source();
}
