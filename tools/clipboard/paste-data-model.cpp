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

#include "paste-data-model.h"

#include <QDebug>
#include <QTimer>

#include "paste-data-provider.h"

PasteDataModel::PasteDataModel(QObject* parent)
    : QAbstractListModel(parent),
    m_provider(new PasteDataProvider()),
    m_surfaceId(),
    m_appState(-1),
    m_entriesSelected(0),
    m_anyEntrySelected(false),
    m_allEntriesSelected(false),
    m_shouldUpdateModel(true)
{
    connect(m_provider, SIGNAL(PasteboardChanged()), this, SLOT(onPasteboardChanged()));
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
        roles[ItemDeleted] = "itemDeleted";
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
    case ItemDeleted:
        return entry.itemDeleted;
    default:
        return QVariant();
    }
}

QString PasteDataModel::surfaceId() const
{
    return m_surfaceId;
}

void PasteDataModel::setSurfaceId(QString surfaceId)
{
    if (m_surfaceId != surfaceId) {
        m_surfaceId = surfaceId;
        Q_EMIT(surfaceIdChanged());

        if (!m_surfaceId.isEmpty()) {
            if (m_shouldUpdateModel && m_appState == Qt::ApplicationActive) {
                updateModel();
            }
        }
    }
}

int PasteDataModel::appState() const
{
    return m_appState;
}

void PasteDataModel::setAppState(int state)
{
    if (m_appState != state) {
        m_appState = state;
        Q_EMIT(appStateChanged());

        if (m_appState == Qt::ApplicationActive) {
            if (m_shouldUpdateModel && !m_surfaceId.isEmpty()) {
                updateModel();
            }
        }
    }
}

bool PasteDataModel::anyEntrySelected() const
{
    return m_anyEntrySelected;
}

bool PasteDataModel::allEntriesSelected() const
{
    return m_allEntriesSelected;
}

void PasteDataModel::setAllEntriesSelected(bool selected)
{
    for (int i = 0; i < m_entries.size(); ++i) {
        setEntrySelectedByIndex(i, selected);
    }
}

void PasteDataModel::setEntrySelectedByIndex(int index, bool selected)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    if (m_entries[index].itemSelected != selected) {
        QVector<int> roles;
        roles << ItemSelected;

        m_entries[index].itemSelected = selected;

        Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), roles);

        // Keep count of how many entries are selected
        if (selected) {
            m_entriesSelected++;
        } else {
            m_entriesSelected--;
        }

        // Verify if any entry is selected and notify
        if (m_entriesSelected > 0 && !m_anyEntrySelected) {
            m_anyEntrySelected = true;
            Q_EMIT anyEntrySelectedChanged();
        } else if (m_entriesSelected <= 0 && m_anyEntrySelected) {
            m_anyEntrySelected = false;
            Q_EMIT anyEntrySelectedChanged();
        }

        // Verify if all entries are selected and notify
        if (m_entriesSelected == rowCount() && !m_allEntriesSelected) {
            m_allEntriesSelected = true;
            Q_EMIT allEntriesSelectedChanged();
        } else if (m_entriesSelected != rowCount() && m_allEntriesSelected) {
            m_allEntriesSelected = false;
            Q_EMIT allEntriesSelectedChanged();
        }
    }
}

void PasteDataModel::saveEntriesDeleted()
{
    for (int i = m_entries.size() - 1; i >= 0; --i) {
        if (m_entries[i].itemDeleted) {
            removeEntryByIndex(i);
        }
    }
}

void PasteDataModel::cancelEntriesDeleted()
{
    for (int i = 0; i < m_entries.size(); ++i) {
        setEntryDeletedByIndex(i, false);
    }
}

void PasteDataModel::setSelectedEntriesDeleted()
{
    QList<int> idxs;
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].itemSelected) {
            idxs << i;
            setEntrySelectedByIndex(i, false);
        }
    }

    for (int i = 0; i < idxs.size(); ++i) {
        setEntryDeletedByIndex(idxs[i], true);
    }
}

void PasteDataModel::setEntryDeletedByIndex(int index, bool deleted)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    if (m_entries[index].itemDeleted != deleted) {
        QVector<int> roles;
        roles << ItemDeleted;

        m_entries[index].itemDeleted = deleted;

        Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), roles);
    }
}

void PasteDataModel::addEntryByPasteId(const QString& pasteId)
{
    PasteDataEntry entry;

    int id = pasteId.toInt();
    entry.pasteId = QString::number(id);
    entry.source = m_provider->pasteSourceById(m_surfaceId, id);

    QMimeData *pasteMimeData = m_provider->pasteDataById(m_surfaceId, id);
    entry.dataType = TextType;
    entry.pasteData = pasteMimeData->text();

    entry.itemSelected = false;
    entry.itemDeleted = false;
    addEntry(entry);
}

void PasteDataModel::addEntry(PasteDataEntry& entry)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.prepend(entry);
    endInsertRows();
    Q_EMIT rowCountChanged();
}

void PasteDataModel::removeEntry(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    Q_EMIT rowCountChanged();
}

void PasteDataModel::removeEntryByIndex(int index)
{
    if (index < 0 || index >= m_entries.size())
        return;

    if (!m_provider->removePaste(m_surfaceId, m_entries[index].pasteId.toInt())) 
        return;

    removeEntry(index);
}

void PasteDataModel::onPasteboardChanged()
{
    m_shouldUpdateModel = true;
}

void PasteDataModel::updateModel()
{
    QStringList pasteData = m_provider->allPasteIds(m_surfaceId);

    for (int i = m_entries.size() - 1; i >= 0; i--) {
        if (pasteData.removeAll(m_entries[i].pasteId) == 0) {
            // Paste id was removed from current pasteboard
            removeEntry(i);
        }
    }

    for (int i = 0; i < pasteData.size(); ; ++i) {
        addEntryByPasteId(pasteData.at(i));
    }

    m_shouldUpdateModel = false;
}
