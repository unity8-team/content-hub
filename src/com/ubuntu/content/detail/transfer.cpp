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

#include "transfer.h"

namespace cucd = com::ubuntu::content::detail;

struct cucd::Transfer::Private
{
};

cucd::Transfer::Transfer(QObject* parent) : QObject(parent), d(new Private())
{
}

cucd::Transfer::~Transfer()
{
}

int cucd::Transfer::State()
{
    return -1;
}

void cucd::Transfer::Abort()
{
}

void cucd::Transfer::Start()
{
}

void cucd::Transfer::Charge(const QStringList&)
{
}

QStringList cucd::Transfer::Collect()
{
    return QStringList{};
}
