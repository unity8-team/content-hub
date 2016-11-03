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

#ifndef PASTEDATAMODEL_H
#define PASTEDATAMODEL_H

#include <QAbstractListModel>

class PasteDataModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(bool anyEntrySelected READ anyEntrySelected NOTIFY anyEntrySelectedChanged)
    Q_PROPERTY(bool allEntriesSelected READ allEntriesSelected NOTIFY allEntriesSelectedChanged)

    Q_ENUMS(Roles)
    Q_ENUMS(PasteDataType)

public:
    PasteDataModel(QObject* parent=0);
    ~PasteDataModel();

    enum Roles {
        PasteId = Qt::UserRole + 1,
        Source,
        DataType,
        PasteData,
        ItemSelected,
        ItemDeleted
    };

    enum PasteDataType {
        TextType,
        ImageType
    };

    // reimplemented from QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent=QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    bool anyEntrySelected() const;
    bool allEntriesSelected() const;

    Q_INVOKABLE void setAllEntriesSelected(bool selected);
    Q_INVOKABLE void setEntrySelectedByIndex(int index, bool selected);
    Q_INVOKABLE void saveEntriesDeleted();
    Q_INVOKABLE void cancelEntriesDeleted();
    Q_INVOKABLE void setSelectedEntriesDeleted();
    Q_INVOKABLE void setEntryDeletedByIndex(int index, bool deleted);
    Q_INVOKABLE void pasteEntryByIndex(int index);
    Q_INVOKABLE void removeEntryByIndex(int index);

protected:
    struct PasteDataEntry {
        QString pasteId;
        QString source;
        PasteDataType dataType;
        QString pasteData;
        bool itemSelected;
        bool itemDeleted;
    };
    QList<PasteDataEntry> m_entries;

protected Q_SLOTS:
    virtual void populateModel();

Q_SIGNALS:
    void rowCountChanged();
    void anyEntrySelectedChanged();
    void allEntriesSelectedChanged();

private:
    int m_entriesSelected;
    bool m_anyEntrySelected;
    bool m_allEntriesSelected;

    void addEntry(PasteDataEntry& entry);
};

#endif // PASTEDATAMODEL_H
