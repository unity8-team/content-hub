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

#include <com/ubuntu/content/transfer.h>

#include "transfer_p.h"

namespace cuc = com::ubuntu::content;

cuc::Transfer::Transfer(const QSharedPointer<cuc::Transfer::Private>& d, QObject* parent)
        : QObject(parent),
          d(d)
{
    QObject::connect(d->remote_transfer,
                SIGNAL (StateChanged(int)),
                this,
                SIGNAL (stateChanged()));
    QObject::connect(d->remote_transfer,
                SIGNAL (SelectionTypeChanged(int)),
                this,
                SIGNAL (selectionTypeChanged()));
}

cuc::Transfer::~Transfer()
{
}

cuc::Transfer::State cuc::Transfer::state() const
{
    return d->state();
}

bool cuc::Transfer::start()
{
    return d->start();
}

bool cuc::Transfer::abort()
{
    return d->abort();
}

bool cuc::Transfer::charge(const QVector<cuc::Item>& items)
{
    return d->charge(items);
}

QVector<cuc::Item> cuc::Transfer::collect()
{
    return d->collect();
}

cuc::Transfer::SelectionType cuc::Transfer::selectionType() const
{
    return d->selection_type();
}

bool cuc::Transfer::setSelectionType(const cuc::Transfer::SelectionType& type)
{
    return d->setSelectionType(type);
}
