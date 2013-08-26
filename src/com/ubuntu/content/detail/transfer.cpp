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

#include <com/ubuntu/content/transfer.h>

#include <QDebug>

namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

struct cucd::Transfer::Private
{
    Private() : state(cuc::Transfer::initiated)
    {
    }
    
    cuc::Transfer::State state;
    QStringList items;
};

cucd::Transfer::Transfer(QObject* parent) : QObject(parent), d(new Private())
{
    qDebug() << __PRETTY_FUNCTION__;
}

cucd::Transfer::~Transfer()
{
}

int cucd::Transfer::State()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->state;
}

void cucd::Transfer::Abort()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state != cuc::Transfer::aborted)
        return;

    d->state = cuc::Transfer::aborted;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Start()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state != cuc::Transfer::in_progress)
        return;

    d->state = cuc::Transfer::in_progress;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Charge(const QStringList& items)
{
    qDebug() << __PRETTY_FUNCTION__;
    d->items = items;

    if (d->state != cuc::Transfer::charged)
        return;

    d->state = cuc::Transfer::charged;
    Q_EMIT(StateChanged(d->state));
}

QStringList cucd::Transfer::Collect()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state != cuc::Transfer::collected)
    {
        d->state = cuc::Transfer::collected;
        Q_EMIT(StateChanged(d->state));
    }

    return d->items;
}
