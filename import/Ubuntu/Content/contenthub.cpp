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
#include <contentpeer.h>
#include <contenttransfer.h>
#include <contenttype.h>

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>

#include <QDebug>

/*!
 * \qmltype ContentHub
 * \instantiates ContentHub
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief FIXME documentation
 *
 * FIXME documentation
 *
 * Example usage:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Content 0.1
 *
 * Rectangle {
 *     width: units.gu(40)
 *     height: units.gu(20)
 *
 *     Button {
 *         text: "Import"
 *          onClicked: {
 *              var transfer = ContentHub.importContent(ContentType.Pictures)
 *         }
 *     }
 * }
 * \endqml
 *
 */

namespace cuc = com::ubuntu::content;

ContentHub::ContentHub(QObject *parent)
    : QObject(parent),
      m_hub(0)
{
    m_hub = cuc::Hub::Client::instance();
//    FIXME write the import export handler
//    m_hub->register_import_export_handler(handler);
}

/*!
 * \qmlmethod ContentHub::defaultSourceForType()
 *
 *  Returns the default peer for the given content \a type
 */
ContentPeer *ContentHub::defaultSourceForType(int type)
{
    qDebug() << Q_FUNC_INFO;

    const cuc::Type &hubType = contentType2HubType(type);
    cuc::Peer hubPeer = m_hub->default_peer_for_type(hubType);

    ContentPeer *qmlPeer = new ContentPeer(this);
    qmlPeer->setName(hubPeer.id());
//    FIXME set the proper type
//    contentPeer->setType(hubPeer.id());

    return qmlPeer;
}

/*!
 * \qmlmethod ContentHub::knownSourcesForType()
 *
 *  Returns all possible peers for the given content \a type
 */
QList<ContentPeer *> ContentHub::knownSourcesForType(int type)
{
    qDebug() << Q_FUNC_INFO;

    const cuc::Type &hubType = contentType2HubType(type);
    QVector<cuc::Peer> hubPeers = m_hub->known_peers_for_type(hubType);

    QList<ContentPeer *> qmlPeers;
    foreach (const cuc::Peer &hubPeer, hubPeers) {
        ContentPeer *qmlPeer = new ContentPeer(this);
        qmlPeer->setName(hubPeer.id());
        //    FIXME set the proper type
        //    contentPeer->setType(hubPeer.id());
        qmlPeers.append(qmlPeer);
    }

    return qmlPeers;
}

/*!
 * \qmlmethod ContentHub::importContent()
 *
 * Start a request to import data of \a type from a peer, the user needs to select
 */
ContentTransfer *ContentHub::importContent(int type)
{
    qDebug() << Q_FUNC_INFO << static_cast<ContentType::Type>(type);

    const cuc::Type &hubType = contentType2HubType(type);
//    FIXME show user a selection of possible peers
    cuc::Peer hubPeer = m_hub->default_peer_for_type(hubType);
    cuc::Transfer *hubTransfer = m_hub->create_import_for_type_from_peer(hubType, hubPeer);
    ContentTransfer *qmlTransfer = new ContentTransfer(this);
    qmlTransfer->setTransfer(hubTransfer);
    return qmlTransfer;
}

/*!
 * \qmlmethod ContentHub::importContent()
 *
 * Start a request to import data of \a type from the given \peer
 */
ContentTransfer *ContentHub::importContent(int type, ContentPeer *peer)
{
    qDebug() << Q_FUNC_INFO << static_cast<ContentType::Type>(type) << peer;

    const cuc::Type &hubType = contentType2HubType(type);
//    FIXME convert from peer, instead of using the default
    cuc::Peer hubPeer = m_hub->default_peer_for_type(hubType);
    cuc::Transfer *hubTransfer = m_hub->create_import_for_type_from_peer(hubType, hubPeer);
    ContentTransfer *qmlTransfer = new ContentTransfer(this);
    qmlTransfer->setTransfer(hubTransfer);
    return qmlTransfer;
}

/*!
 * \qmlmethod ContentHub::restoreImports()
 *
 *  FIXME add documentation
 */
void ContentHub::restoreImports()
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty list<ContentTransfer> ContentHub::finishedImports
 *
 * FIXME add documentation
 */
QQmlListProperty<ContentTransfer> ContentHub::finishedImports()
{
    qDebug() << Q_FUNC_INFO;
    return QQmlListProperty<ContentTransfer>(this, m_finishedImports);
}

/*!
 * \brief ContentHub::conentType2HubType converts a ContentType::Type to a
 * com::ubuntu::content::Type
 * \param type integer representing a ContentType::Type
 * \return
 */
const com::ubuntu::content::Type &ContentHub::contentType2HubType(int type) const
{
    switch(type) {
    case 1: return cuc::Type::Known::documents();
    case 2: return cuc::Type::Known::pictures();
    case 3: return cuc::Type::Known::music();
    default: return cuc::Type::unknown();
    }
}
