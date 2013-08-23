/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COM_UBUNTU_CONTENTHUB_H_
#define COM_UBUNTU_CONTENTHUB_H_

#include <QList>
#include <QObject>
#include <QQmlListProperty>

class ContentPeer;
class ContentTransfer;
class QmlImportExportHandler;

namespace com {
namespace ubuntu {
namespace content {
class Hub;
class Type;
class Transfer;
}
}
}

class ContentHub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ContentTransfer> finishedImports READ finishedImports NOTIFY finishedImportsChanged)

public:
    ContentHub(QObject *parent = nullptr);

    Q_INVOKABLE ContentPeer *defaultSourceForType(int type);
    Q_INVOKABLE QList<ContentPeer *> knownSourcesForType(int type);

    Q_INVOKABLE ContentTransfer* importContent(int type);
    Q_INVOKABLE ContentTransfer* importContent(int type, ContentPeer *peer);

    Q_INVOKABLE void restoreImports();

    QQmlListProperty<ContentTransfer> finishedImports();

Q_SIGNALS:
    void exportRequested(ContentTransfer *transfer);
    void finishedImportsChanged();

private Q_SLOTS:
    void handleImport(com::ubuntu::content::Transfer * transfer);
    void handleExport(com::ubuntu::content::Transfer * transfer);

private:
    const com::ubuntu::content::Type &contentType2HubType(int type) const;

    QList<ContentTransfer *> m_finishedImports;

    com::ubuntu::content::Hub *m_hub;
    QmlImportExportHandler *m_handler;
};

#endif // COM_UBUNTU_CONTENTHUB_H_
