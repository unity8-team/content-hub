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

#include "debug.h"
#include "paste.h"
#include "utils.cpp"

#include <QFileInfo>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/paste.h>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/manager.h>

namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

struct cucd::Paste::Private
{
    Private(const int id,
            const QString& source):
        state(cuc::Paste::created),
            id(id),
            source(source)
    {
    }
    
    cuc::Paste::State state;
    const int id;
    const QString source;
    QString destination;
    QVariantList items;
};

cucd::Paste::Paste(const int id,
                         const QString& source,
                         QObject* parent) :
    QObject(parent), d(new Private(id, source))
{
    TRACE() << __PRETTY_FUNCTION__;
}

cucd::Paste::~Paste()
{
    TRACE() << __PRETTY_FUNCTION__;
}

/* unique id of the paste */
int cucd::Paste::Id()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->id;
}

/* returns the add_id of the source app */
QString cucd::Paste::source()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->source;
}

/* returns the add_id of the destination app */
QString cucd::Paste::destination()
{
    TRACE() << __PRETTY_FUNCTION__;
    if (d->destination.isEmpty())
        return d->source;
    return d->destination;
}

void cucd::Paste::setDestination(QString& dest)
{
    TRACE() << __PRETTY_FUNCTION__;
    d->destination = dest;
}

/* returns the object path for the paste */
QString cucd::Paste::path()
{
    TRACE() << Q_FUNC_INFO << "destination:" << destination();
    static const QString path_pattern{"/pastes/%1/%2"};
    QString path = path_pattern
            .arg(sanitize_id(destination()))
            .arg(d->id);
    return path;
}

int cucd::Paste::State()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->state;
}

void cucd::Paste::Abort()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Paste::aborted)
        return;

    d->items.clear();
    d->state = cuc::Paste::aborted;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Paste::Charge(const QVariantList& items)
{
    TRACE() << __PRETTY_FUNCTION__ << "STATE:" << d->state;

    if (d->state == cuc::Paste::charged)
        return;

    QVariantList ret;
    Q_FOREACH(QVariant iv, items) {
        cuc::Item item = qdbus_cast<Item>(iv);
        ret.append(QVariant::fromValue(item));
    }
    if (ret.count() <= 0) {
        qWarning() << "Failed to charge items, aborting";
        d->state = cuc::Paste::aborted;
    } else {
        d->items = ret;
        d->state = cuc::Paste::charged;
    }
    Q_EMIT(StateChanged(d->state));
}

QVariantList cucd::Paste::Collect()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state != cuc::Paste::collected)
    {
        d->state = cuc::Paste::collected;
        Q_EMIT(StateChanged(d->state));
    }

    return d->items;
}

void cucd::Paste::Finalize()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Paste::finalized)
        return;

    d->items.clear();
    d->state = cuc::Paste::finalized;
    Q_EMIT(StateChanged(d->state));
}
