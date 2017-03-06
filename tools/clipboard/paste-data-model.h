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

class PasteDataProvider;

class PasteDataModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(QString surfaceId READ surfaceId WRITE setSurfaceId NOTIFY surfaceIdChanged)
    Q_PROPERTY(int appState READ appState WRITE setAppState NOTIFY appStateChanged)
    Q_PROPERTY(bool anyEntrySelected READ anyEntrySelected NOTIFY anyEntrySelectedChanged)
    Q_PROPERTY(bool allEntriesSelected READ allEntriesSelected NOTIFY allEntriesSelectedChanged)

    Q_ENUMS(Roles)
    Q_ENUMS(PasteDataType)
    Q_ENUMS(PasteOutputType)

public:
    PasteDataModel(QObject* parent=0);
    ~PasteDataModel();

    enum Roles {
        Id = Qt::UserRole + 1,
        Source,
        DataType,
        TextData,
        HtmlData,
        ImageData,
        EntrySelected,
        Deleted,
        OutputType 
    };

    enum PasteDataType {
        Text,
        Image,
        ImageUrl
    };

    enum PasteOutputType {
        PlainText,
        RichText
    };

    // reimplemented from QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent=QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    QString surfaceId() const; 
    void setSurfaceId(QString surfaceId); 

    int appState() const; 
    void setAppState(int state); 

    bool anyEntrySelected() const;
    bool allEntriesSelected() const;

    Q_INVOKABLE void selectAll(bool selected);
    Q_INVOKABLE void selectByIndex(int index, bool selected);
    Q_INVOKABLE void markSelectedForDeletion();
    Q_INVOKABLE void markForDeletionByIndex(int index, bool deleted);
    Q_INVOKABLE void cancelDeletion();
    Q_INVOKABLE void deleteEntries();
    Q_INVOKABLE void deleteByIndex(int index);
    Q_INVOKABLE void setOutputTypeByIndex(int index, PasteOutputType outputType);

protected:
    struct PasteEntry {
        QString id;
        QString source;
        PasteDataType dataType;
        QString textData;
        QString htmlData;
        QString imageData;
        bool entrySelected;
        bool deleted;
        PasteOutputType outputType;
    };
    QList<PasteEntry> m_entries;


Q_SIGNALS:
    void rowCountChanged();
    void surfaceIdChanged();
    void appStateChanged();
    void anyEntrySelectedChanged();
    void allEntriesSelectedChanged();

private Q_SLOTS:
    void onPasteboardChanged();

private:
    void addToModelByPasteId(const QString& pasteId);
    void addToModel(PasteEntry& entry);
    void removeFromModel(int index);
    void updateModel();

    PasteDataProvider* m_provider;
    QString m_surfaceId;
    int m_appState;
    int m_selectedEntries;
    bool m_shouldUpdateModel;
};

#endif // PASTEDATAMODEL_H
