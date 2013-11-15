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

#include "contenthub.h"
#include "contentpeer.h"
#include "contentstore.h"
#include "contenttransfer.h"
#include "contenttype.h"
#include "qmlimportexporthandler.h"

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>

#include <QStandardPaths>
#include <QDebug>

/*!
 * \qmltype ContentHub
 * \instantiates ContentHub
 * \inqmlmodule Ubuntu.Content 0.1
 *
 * Example usage for importing content:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 * import Ubuntu.Content 0.1
 *
 * MainView {
 *     id: root
 *     width: units.gu(60)
 *     height: units.gu(90)
 *     Button {
 *         anchors {
 *             left: parent.left
 *             margins: units.gu(2)
 *          }
 *         text: "Import from default"
 *          onClicked: {
 *              var peer = ContentHub.defaultSourceForType(ContentType.Pictures);
 *              activeTransfer = ContentHub.importContent(ContentType.Pictures, peer);
 *         }
 *     }
 *     Button {
 *         anchors {
 *             right: parent.right
 *             margins: units.gu(2)
 *          }
 *         text: "Import from a selectable list"
 *          onClicked: {
 *              activeTransfer = ContentHub.importContent(ContentType.Pictures);
 *              activeTransfer.selectionType =ContentTransfer.Multiple;
 *              activeTransfer.start();
 *         }
 *     }
 *     ContentImportHint {
 *         id: importHint
 *         anchors.fill: parent
 *         activeTransfer: root.activeTransfer
 *     }
 *     property list<ContentItem> importItems
 *     property var activeTransfer
 *     Connections {
 *         target: root.activeTransfer
 *         onStateChanged: {
 *             if (root.activeTransfer.state === ContentTransfer.Charged)
 *                 importItems = root.activeTransfer.items;
 *         }
 *     }
 * }
 * \endqml
 *
 * Example usage for providing a content export:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Content 0.1
 *
 * Rectangle {
 *     property list<ContentItem> selectedItems
 *     Connections {
 *         target: ContentHub
 *         onExportRequested: {
 *             // show content picker
 *             transfer.items = selectedItems;
 *             transfer.state = ContentTransfer.Charged;
 *         }
 *     }
 * }
 * \endqml
 */

namespace cuc = com::ubuntu::content;

ContentHub::ContentHub(QObject *parent)
    : QObject(parent),
      m_hub(0)
{
    m_hub = cuc::Hub::Client::instance();
    m_handler = new QmlImportExportHandler(this);
    m_hub->register_import_export_handler(m_handler);

    connect(m_handler, SIGNAL(importRequested(com::ubuntu::content::Transfer*)),
            this, SLOT(handleImport(com::ubuntu::content::Transfer*)));
    connect(m_handler, SIGNAL(exportRequested(com::ubuntu::content::Transfer*)),
            this, SLOT(handleExport(com::ubuntu::content::Transfer*)));
}

/*!
 * \qmlmethod ContentHub::defaultSourceForType(ContentType)
 *
 *  Returns the default \a ContentPeer for the given \a ContentType
 */
ContentPeer *ContentHub::defaultSourceForType(int type)
{
    qDebug() << Q_FUNC_INFO;

    const cuc::Type &hubType = ContentType::contentType2HubType(type);
    cuc::Peer hubPeer = m_hub->default_peer_for_type(hubType);

    ContentPeer *qmlPeer = new ContentPeer(this);
    qmlPeer->setPeer(hubPeer);

    return qmlPeer;
}

/*!
 * \qmlmethod ContentHub::defaultStoreForType(ContentType)
 *
 *  Returns the default \a ContentStore for the given \a ContentType
 */
ContentStore *ContentHub::defaultStoreForType(int type)
{
    qDebug() << Q_FUNC_INFO;

    const cuc::Type &hubType = ContentType::contentType2HubType(type);
    const cuc::Store *hubStore = m_hub->store_for_scope_and_type(cuc::app, hubType);

    qDebug() << Q_FUNC_INFO << "STORE:" << hubStore->uri();

    ContentStore *qmlStore = new ContentStore(this);
    qmlStore->setStore(hubStore);

    return qmlStore;
}

/*!
 * \qmlmethod ContentHub::knownSourcesForType(ContentType)
 *
 *  Returns all possible peers for the given ContentType
 */
QVariantList ContentHub::knownSourcesForType(int type)
{
    qDebug() << Q_FUNC_INFO;

    const cuc::Type &hubType = ContentType::contentType2HubType(type);
    QVector<cuc::Peer> hubPeers = m_hub->known_peers_for_type(hubType);

    QVariantList qmlPeers;
    foreach (const cuc::Peer &hubPeer, hubPeers) {
        ContentPeer *qmlPeer = new ContentPeer(this);
        qmlPeer->setPeer(hubPeer);
        qmlPeers.append(QVariant::fromValue(qmlPeer));
    }
    return qmlPeers;
}

/*!
 * \qmlmethod ContentHub::importContent(ContentType)
 * \overload ContentHub::importContent(ContentType, ContentPeer)
 *
 * \brief Request to import data of \a ContentType from the default
 * ContentPeer
 */
ContentTransfer *ContentHub::importContent(int type)
{
    qDebug() << Q_FUNC_INFO << static_cast<ContentType::Type>(type);

    const cuc::Type &hubType = ContentType::contentType2HubType(type);
//    FIXME show user a selection of possible peers instead
    cuc::Peer hubPeer = m_hub->default_peer_for_type(hubType);

    return importContent(hubType, hubPeer);
}

/*!
 * \qmlmethod ContentHub::importContent(ContentType, ContentPeer)
 * \overload ContentHub::importContent(ContentType)
 *
 * \brief Request to import data of \a ContentType from the
 * specified \a ContentPeer
 */
ContentTransfer *ContentHub::importContent(int type, ContentPeer *peer)
{
    qDebug() << Q_FUNC_INFO << static_cast<ContentType::Type>(type) << peer;

    const cuc::Type &hubType = ContentType::contentType2HubType(type);
    return importContent(hubType, peer->peer());
}

/*!
 * \brief ContentHub::importContent creates a ContentTransfer object
 * \a type
 * \a peer
 * \internal
 */
ContentTransfer* ContentHub::importContent(const com::ubuntu::content::Type &hubType,
                                           const com::ubuntu::content::Peer &hubPeer)
{
    cuc::Transfer *hubTransfer = m_hub->create_import_for_type_from_peer(hubType, hubPeer);
// FIXME update tests so this can be enabled
//    if (!hubTransfer)
//        return nullptr;

    ContentTransfer *qmlTransfer = new ContentTransfer(this);
    qmlTransfer->setTransfer(hubTransfer, ContentTransfer::Import);
    m_activeImports.insert(hubTransfer, qmlTransfer);
    return qmlTransfer;
}

/*!
 * \qmlmethod ContentHub::restoreImports()
 * \internal
 */
void ContentHub::restoreImports()
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty list<ContentTransfer> ContentHub::finishedImports
 * \internal
 */
QQmlListProperty<ContentTransfer> ContentHub::finishedImports()
{
    qDebug() << Q_FUNC_INFO;
    return QQmlListProperty<ContentTransfer>(this, m_finishedImports);
}

/*!
 * \brief ContentHub::handleImport handles an incoming request for importing content
 * \internal
 */
void ContentHub::handleImport(com::ubuntu::content::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO;
    if (transfer->store().uri().isEmpty())
    {
        const cuc::Store *store = new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/HubIncoming/" + QString::number(transfer->id()), this};
        transfer->setStore(store);
    }
    ContentTransfer *qmlTransfer = nullptr;
    if (m_activeImports.contains(transfer)) {
        qmlTransfer = m_activeImports.take(transfer);
        qmlTransfer->collectItems();
    } else {
        qmlTransfer = new ContentTransfer(this);
        qmlTransfer->setTransfer(transfer, ContentTransfer::Import);
    }

    m_finishedImports.append(qmlTransfer);
    Q_EMIT finishedImportsChanged();
}

/*!
 * \brief ContentHub::handleExport handles an incoming request for exporting content
 * \internal
 */
void ContentHub::handleExport(com::ubuntu::content::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO;
    ContentTransfer *qmlTransfer = new ContentTransfer(this);
    qmlTransfer->setTransfer(transfer, ContentTransfer::Export);

    Q_EMIT exportRequested(qmlTransfer);
}

/*!
 * \qmlsignal ContentHub::exportRequested(ContentTransfer transfer)
 *
 * The signal is triggered when an export is requested.
 */

