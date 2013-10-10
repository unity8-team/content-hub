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

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/transfer.h>

#include <QDebug>

namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

struct cucd::Transfer::Private
{
    Private(const int id,
            const QString& source,
            const QString& destination) :
        state(cuc::Transfer::created),
            id(id),
            source(source),
            destination(destination),
            selection_type(cuc::Transfer::single),
            source_started_by_content_hub(false)
    {
    }
    
    cuc::Transfer::State state;
    const int id;
    const QString source;
    const QString destination;
    QString store;
    int selection_type;
    QStringList items;
    bool source_started_by_content_hub;
};

cucd::Transfer::Transfer(const int id,
                         const QString& source,
                         const QString& destination,
                         QObject* parent) :
    QObject(parent), d(new Private(id, source, destination))
{
    qDebug() << __PRETTY_FUNCTION__;
}

cucd::Transfer::~Transfer()
{
    qDebug() << __PRETTY_FUNCTION__;
    purge_store_cache(d->store);
}

/* unique id of the transfer */
int cucd::Transfer::Id()
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

    purge_store_cache(d->store);
    d->items.clear();
    d->state = cuc::Transfer::aborted;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Start()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::initiated)
        return;

    d->state = cuc::Transfer::initiated;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Handled()
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

    QStringList ret;
    Q_FOREACH(QString i, items)
        ret.append(copy_to_store(i, d->store));

    Q_FOREACH(QString f, ret)
        qDebug() << Q_FUNC_INFO << "Item:" << f;

    if (ret.count() <= 0)
    {
        qWarning() << "Failed to charge items, aborting";
        d->state = cuc::Transfer::aborted;
    }
    else
    {
        d->items = ret;
        d->state = cuc::Transfer::charged;
    }
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

void cucd::Transfer::Finalize()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::finalized)
        return;

    purge_store_cache(d->store);
    d->items.clear();
    d->state = cuc::Transfer::finalized;
    Q_EMIT(StateChanged(d->state));
}

QString cucd::Transfer::Store()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->store;
}

void cucd::Transfer::SetStore(QString uri)
{
    qDebug() << Q_FUNC_INFO;

    if (d->store == uri)
        return;

    d->store = uri;
    Q_EMIT(StoreChanged(d->store));
}

int cucd::Transfer::SelectionType()
{
    qDebug() << __PRETTY_FUNCTION__;
    return d->selection_type;
}

void cucd::Transfer::SetSelectionType(int type)
{
    qDebug() << Q_FUNC_INFO;
    if (d->state != cuc::Transfer::created)
        return;
    if (d->selection_type == type)
        return;

    d->selection_type = type;
    Q_EMIT(SelectionTypeChanged(d->selection_type));
}

/* returns the object path for the export */
QString cucd::Transfer::export_path()
{
    qDebug() << Q_FUNC_INFO << "source:" << d->source;
    static const QString exporter_path_pattern{"/transfers/%1/export/%2"};
    QString source = exporter_path_pattern
            .arg(sanitize_id(d->source))
            .arg(d->id);
    return source;
}

/* returns the object path for the import */
QString cucd::Transfer::import_path()
{
    qDebug() << Q_FUNC_INFO << "destination:" << d->destination;
    static const QString importer_path_pattern{"/transfers/%1/import/%2"};
    QString destination = importer_path_pattern
            .arg(sanitize_id(d->destination))
            .arg(d->id);
    return destination;
}

/* sets, if the source app is freshly started by the content hub */
void cucd::Transfer::SetSourceStartedByContentHub(bool started)
{
    d->source_started_by_content_hub = started;
}

/* returns if the source app was started by the content hub */
bool com::ubuntu::content::detail::Transfer::WasSourceStartedByContentHub() const
{
    return d->source_started_by_content_hub;
}
