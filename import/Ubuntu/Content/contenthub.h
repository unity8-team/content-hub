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
#include "contentpeer.h"
#include "contenttransfer.h"

class ContentStore;
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
    ContentHub(const ContentHub&) = delete;

    static ContentHub *instance();

    Q_INVOKABLE void restoreImports();

    QQmlListProperty<ContentTransfer> finishedImports();

    Q_INVOKABLE ContentTransfer* importContent(com::ubuntu::content::Peer peer, ContentType::Type type);
    Q_INVOKABLE ContentTransfer* exportContent(com::ubuntu::content::Peer peer, ContentType::Type type);
    Q_INVOKABLE ContentTransfer* shareContent(com::ubuntu::content::Peer peer, ContentType::Type type);

Q_SIGNALS:
    void importRequested(ContentTransfer *transfer);
    void exportRequested(ContentTransfer *transfer);
    void shareRequested(ContentTransfer *transfer);

    void finishedImportsChanged();

private Q_SLOTS:
    void handleImport(com::ubuntu::content::Transfer* transfer);
    void handleExport(com::ubuntu::content::Transfer* transfer);
    void handleShare(com::ubuntu::content::Transfer* transfer);
    void updateState();

private:
    QString setupPromptSession();
    QList<ContentTransfer *> m_finishedImports;
    QHash<com::ubuntu::content::Transfer *, ContentTransfer *> m_activeImports;
    com::ubuntu::content::Hub *m_hub;
    QmlImportExportHandler *m_handler;

protected:
    ContentHub(QObject* = nullptr);

};

#endif // COM_UBUNTU_CONTENTHUB_H_
