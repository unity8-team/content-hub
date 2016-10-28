/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include <QTimer>
#include <QDebug>

#include "paste-data-model.h"

PasteDataModel::PasteDataModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // This way populateModel can be override during tests
    QTimer::singleShot(0, this, SLOT(populateModel()));
}

PasteDataModel::~PasteDataModel()
{
}

QHash<int, QByteArray> PasteDataModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[PasteId] = "pasteId";
        roles[Source] = "source";
        roles[DataType] = "dataType";
        roles[PasteData] = "pasteData";
        roles[ItemSelected] = "itemSelected";
        roles[Deleted] = "deleted";
    }
    return roles;
}

int PasteDataModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_entries.count();
}

QVariant PasteDataModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const PasteDataEntry& entry = m_entries.at(index.row());
    switch (role) {
    case PasteId:
        return entry.pasteId;
    case Source:
        return entry.source;
    case DataType:
        return entry.dataType;
    case PasteData:
        return entry.pasteData;
    case ItemSelected:
        return entry.itemSelected;
    case Deleted:
        return entry.deleted;
    default:
        return QVariant();
    }
}

void PasteDataModel::setAllEntriesSelected(bool selected)
{
    QVector<int> roles;
    roles << ItemSelected;

    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].itemSelected != selected) {
            m_entries[i].itemSelected = selected;
            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), roles);
        }
    }
}

void PasteDataModel::setEntrySelectedByIndex(int index, bool selected)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    if (m_entries[index].itemSelected != selected) {
        m_entries[index].itemSelected = selected;

        QVector<int> roles;
        roles << ItemSelected;
        Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), roles);
    }
}

void PasteDataModel::cancelEntriesDeleted()
{
    QVector<int> roles;
    roles << Deleted;

    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].deleted) {
            m_entries[i].deleted = false;
            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), roles);
        }
    }
}

void PasteDataModel::saveEntriesDeleted()
{
    for (int i = m_entries.size() - 1; i >= 0; --i) {
        if (m_entries[i].deleted) {
            removeEntryByIndex(i);
        }
    }
}

void PasteDataModel::setSelectedEntriesDeleted()
{
    QVector<int> roles;
    roles << Deleted;

    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].itemSelected && !m_entries[i].deleted) {
            m_entries[i].deleted = true;
            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), roles);
        }
    }
}

void PasteDataModel::pasteEntryByIndex(int index)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    qDebug() << "[TODO] Paste Required (pasteId):" << m_entries[index].pasteId;
}

void PasteDataModel::removeEntryByIndex(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    Q_EMIT rowCountChanged();
}

void PasteDataModel::addEntry(PasteDataEntry& entry)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries << entry;
    endInsertRows();
    Q_EMIT rowCountChanged();
}

void PasteDataModel::populateModel()
{
    //Dummy entries
    PasteDataEntry entry;

    entry.pasteId = "01";
    entry.source = "Messaging";
    entry.dataType = TextType;
    entry.pasteData = "Secrets lie deep within Jupiter";
    entry.itemSelected = false;
    entry.deleted = false;
    addEntry(entry);

    entry.pasteId = "02";
    entry.source = "Dekko";
    entry.dataType = ImageType;
    entry.pasteData = "ubuntu.png";
    entry.itemSelected = false;
    entry.deleted = false;
    addEntry(entry);

    entry.pasteId = "03";
    entry.source = "Dekko";
    entry.dataType = ImageType;
    entry.pasteData = "ubuntu.png";
    entry.itemSelected = false;
    entry.deleted = false;
    addEntry(entry);

    entry.pasteId = "04";
    entry.source = "Notes";
    entry.dataType = TextType;
    entry.pasteData = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
    entry.itemSelected = false;
    entry.deleted = false;
    addEntry(entry);
}
