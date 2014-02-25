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

#include <QHash>
#include <QList>
#include <QObject>
#include <QQmlListProperty>

class ContentPeer;
class ContentStore;
class ContentTransfer;
class QmlImportExportHandler;

namespace com {
namespace ubuntu {
namespace content {
class Hub;
class Peer;
class Store;
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

    Q_INVOKABLE void restoreImports();

    QQmlListProperty<ContentTransfer> finishedImports();

Q_SIGNALS:
    void importRequested(ContentTransfer *transfer);
    void exportRequested(ContentTransfer *transfer);
    void shareRequested(ContentTransfer *transfer);

    void finishedImportsChanged();

private Q_SLOTS:
    void handleImport(com::ubuntu::content::Transfer * transfer);
    void handleExport(com::ubuntu::content::Transfer * transfer);
    void handleShare(com::ubuntu::content::Transfer * transfer);
    void updateState();

private:
    ContentTransfer* importContent(const com::ubuntu::content::Type&,
                                   const com::ubuntu::content::Peer &hubPeer);

    ContentTransfer* exportContent(const com::ubuntu::content::Type&,
                                   const com::ubuntu::content::Peer &hubPeer);
    ContentTransfer* shareContent(const com::ubuntu::content::Type&,
                                   const com::ubuntu::content::Peer &hubPeer);

    QList<ContentTransfer *> m_finishedImports;
    QHash<com::ubuntu::content::Transfer *, ContentTransfer *> m_activeImports;
    com::ubuntu::content::Hub *m_hub;
    QmlImportExportHandler *m_handler;
};

#endif // COM_UBUNTU_CONTENTHUB_H_
