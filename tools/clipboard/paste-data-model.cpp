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
#include <QImage>
#include <QTimer>

#include "paste-data-provider.h"

PasteDataModel::PasteDataModel(QObject* parent)
    : QAbstractListModel(parent),
    m_provider(new PasteDataProvider()),
    m_surfaceId(),
    m_appState(-1),
    m_selectedEntries(0),
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
        roles[Id] = "id";
        roles[Source] = "source";
        roles[DataType] = "dataType";
        roles[TextData] = "textData";
        roles[HtmlData] = "htmlData";
        roles[ImageData] = "imageData";
        roles[Selected] = "selected";
        roles[Deleted] = "deleted";
        roles[OutputType] = "outputType";
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

    const PasteEntry& entry = m_entries.at(index.row());
    switch (role) {
    case Id:
        return entry.id;
    case Source:
        return entry.source;
    case DataType:
        return entry.dataType;
    case TextData:
        return entry.textData;
    case HtmlData:
        return entry.htmlData;
    case ImageData:
        return entry.imageData;
    case Selected:
        return entry.selected;
    case Deleted:
        return entry.deleted;
    case OutputType:
        return entry.outputType;
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
    return m_selectedEntries > 0;
}

bool PasteDataModel::allEntriesSelected() const
{
    return m_selectedEntries == rowCount();
}

void PasteDataModel::selectAll(bool selected)
{
    for (int i = 0; i < m_entries.size(); ++i) {
        selectByIndex(i, selected);
    }
}

void PasteDataModel::selectByIndex(int index, bool selected)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    if (m_entries[index].selected != selected) {
        QVector<int> roles;
        roles << Selected;

        m_entries[index].selected = selected;

        Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), roles);

        // Keep count of how many entries are selected
        if (selected) {
            if (m_selectedEntries == 0) {
                m_selectedEntries++;
                Q_EMIT anyEntrySelectedChanged();
            } else {
                m_selectedEntries++;
            }

            if (m_selectedEntries == rowCount()) {
                Q_EMIT allEntriesSelectedChanged();
            }
        } else {
            if (m_selectedEntries == rowCount()) {
                m_selectedEntries--;
                Q_EMIT allEntriesSelectedChanged();
            } else {
                m_selectedEntries--;
            }

            if (m_selectedEntries == 0) {
                Q_EMIT anyEntrySelectedChanged();
            }
        }
    }
}

void PasteDataModel::markSelectedForDeletion()
{
    QList<int> idxs;
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].selected) {
            idxs << i;
            selectByIndex(i, false);
        }
    }

    for (int i = 0; i < idxs.size(); ++i) {
        markForDeletionByIndex(idxs[i], true);
    }
}

void PasteDataModel::markForDeletionByIndex(int index, bool deleted)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    if (m_entries[index].deleted != deleted) {
        QVector<int> roles;
        roles << Deleted;

        m_entries[index].deleted = deleted;

        Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), roles);
    }
}

void PasteDataModel::cancelDeletion()
{
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].deleted) {
            markForDeletionByIndex(i, false);
        }
    }
}

void PasteDataModel::deleteEntries()
{
    for (int i = m_entries.size() - 1; i >= 0; --i) {
        if (m_entries[i].deleted) {
            deleteByIndex(i);
        }
    }
}

void PasteDataModel::deleteByIndex(int index)
{
    if (index < 0 || index >= m_entries.size())
        return;

    if (!m_provider->removePaste(m_surfaceId, m_entries[index].id.toInt())) 
        return;

    removeFromModel(index);
}

void PasteDataModel::setOutputTypeByIndex(int index, PasteOutputType output)
{
    if (index < 0 || index >= m_entries.size()) {
        return;
    }

    if (m_entries[index].outputType != output) {
        QVector<int> roles;
        roles << OutputType;

        m_entries[index].outputType = output;

        Q_EMIT dataChanged(this->index(index, 0), this->index(index, 0), roles);
    }
}

void PasteDataModel::onPasteboardChanged()
{
    m_shouldUpdateModel = true;
}

void PasteDataModel::addToModelByPasteId(const QString& pasteId)
{
    PasteEntry entry;

    int id = pasteId.toInt();

    entry.id = QString::number(id);
    entry.source = m_provider->pasteSourceById(m_surfaceId, id);

    QMimeData *pasteMimeData = m_provider->pasteDataById(m_surfaceId, id);
    if (pasteMimeData->hasImage()) {
        if (pasteMimeData->imageData().toByteArray().isEmpty()) {
            // Images copied from webborwser-app do not have imageData but in fact an url
            entry.dataType = ImageUrl;
            entry.htmlData = pasteMimeData->html();

            QRegularExpression srcRe("src=\"([^\"]*)\"");
            QRegularExpressionMatch srcMatch = srcRe.match(entry.htmlData);
            if (srcMatch.hasMatch()) {
                entry.imageData = srcMatch.captured(1);
            }

            QRegularExpression altRe("alt=\"([^\"]*)\"");
            QRegularExpressionMatch altMatch = altRe.match(entry.htmlData);
            if (altMatch.hasMatch()) {
                entry.textData = altMatch.captured(1);
            }
        } else {
            entry.dataType = Image;
        }

    } else {
        entry.dataType = Text;
        entry.textData = pasteMimeData->text();
        if (pasteMimeData->hasHtml()) {
            entry.htmlData = pasteMimeData->html();
        }
    }

    entry.selected = false;
    entry.deleted = false;
    entry.outputType = PlainText;

    addToModel(entry);
}

void PasteDataModel::addToModel(PasteEntry& entry)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.prepend(entry);
    endInsertRows();
    Q_EMIT rowCountChanged();
}

void PasteDataModel::removeFromModel(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    Q_EMIT rowCountChanged();
}

void PasteDataModel::updateModel()
{
    QStringList dataList = m_provider->allPasteIds(m_surfaceId);

    for (int i = m_entries.size() - 1; i >= 0; i--) {
        if (dataList.removeAll(m_entries[i].id) == 0) {
            // Paste id was removed from current pasteboard
            removeFromModel(i);
        }
    }

    for (int i = 0; i < dataList.size(); ++i) {
        addToModelByPasteId(dataList.at(i));
    }

    m_shouldUpdateModel = false;
}
