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

#ifndef PASTEDATADELETEDMODEL_H
#define PASTEDATADELETEDMODEL_H

#include <QSortFilterProxyModel>

class PasteDataModel;
class PasteDataEntry;

class PasteDataDeletedModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(PasteDataModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    PasteDataDeletedModel(QObject* parent=0);

    PasteDataModel* sourceModel() const;
    void setSourceModel(PasteDataModel* sourceModel);

    int count() const;

Q_SIGNALS:
    void sourceModelChanged() const;
    void countChanged() const;

protected:
    // reimplemented from QSortFilterProxyModel
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

private Q_SLOTS:
    void onModelChanged();
};

#endif // PASTEDATADELETEDMODEL_H
