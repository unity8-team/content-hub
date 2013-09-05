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
#include "utils.cpp"

#include <com/ubuntu/content/transfer.h>

#include <QDebug>

namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

struct cucd::Transfer::Private
{
    Private(const int id,
            const QString& source,
            const QString& destination,
            const int selection_type) :
        state(cuc::Transfer::initiated),
            id(id),
            source(source),
            destination(destination),
            selection_type(selection_type)
    {
    }
    
    cuc::Transfer::State state;
    const int id;
    const QString source;
    const QString destination;
    const int selection_type;
    QStringList items;
};

cucd::Transfer::Transfer(const int id,
                         const QString& source,
                         const QString& destination,
                         const int selection_type,
                         QObject* parent) :
    QObject(parent), d(new Private(id, source, destination, selection_type))
{
    qDebug() << __PRETTY_FUNCTION__;
}

cucd::Transfer::~Transfer()
{
}

/* unique id of the transfer */
int cucd::Transfer::id()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->id;
}

/* returns the peer_id of the requested export handler */
QString cucd::Transfer::source()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->source;
}

/* returns the peer_id of the application requesting the import */
QString cucd::Transfer::destination()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->destination;
}

int cucd::Transfer::State()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->state;
}

void cucd::Transfer::Abort()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::aborted)
        return;

    d->state = cuc::Transfer::aborted;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Start()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::in_progress)
        return;

    d->state = cuc::Transfer::in_progress;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Charge(const QStringList& items)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::charged)
        return;

    d->items = items;
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

int cucd::Transfer::SelectionType()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->selection_type;
}

/* returns the object path for the export */
QString cucd::Transfer::export_path()
{
    qDebug() << Q_FUNC_INFO;
    static const QString exporter_path_pattern{"/transfers/%1/export/%2"};
    QString source = exporter_path_pattern
            .arg(sanitize_path(d->source))
            .arg(d->id);
    return source;
}

/* returns the object path for the import */
QString cucd::Transfer::import_path()
{
    qDebug() << Q_FUNC_INFO;
    static const QString importer_path_pattern{"/transfers/%1/import/%2"};
    QString destination = importer_path_pattern
            .arg(sanitize_path(d->destination))
            .arg(d->id);
    return destination;
}
