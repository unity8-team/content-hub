/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Arthur Mello <arthur.mello@canonical.com>
 */

#include "paste-data-filter-model.h"

#include "paste-data-model.h"

PasteDataFilterModel::PasteDataFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);

    connect(this, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(onModelChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(onModelChanged()));
}

PasteDataModel* PasteDataFilterModel::sourceModel() const
{
    return qobject_cast<PasteDataModel*>(QSortFilterProxyModel::sourceModel());
}

void PasteDataFilterModel::setSourceModel(PasteDataModel* sourceModel)
{
    if (sourceModel != this->sourceModel()) {
        QSortFilterProxyModel::setSourceModel(sourceModel);
        Q_EMIT sourceModelChanged();
        Q_EMIT countChanged();
    }
}

int PasteDataFilterModel::count() const
{
    return rowCount();
}

bool PasteDataFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    return !sourceModel()->data(index, PasteDataModel::ItemDeleted).toBool();
}

void PasteDataFilterModel::onModelChanged()
{
    Q_EMIT countChanged();
}
