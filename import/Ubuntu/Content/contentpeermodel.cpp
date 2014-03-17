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

#include "contentpeermodel.h"
#include <stdio.h>

#include <QDebug>

namespace cuc = com::ubuntu::content;

/*!
 * \qmltype ContentPeerModel
 * \instantiates ContentPeerModel
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief A list of applications that can export or import a ContentType
 *
 * A ContentPeerModel provides a list of all applications that are registered
 * in the ContentHub as a source or destination of a ContentType
 *
 * See documentation for ContentPeer
 */

ContentPeerModel::ContentPeerModel(QObject *parent)
    : QObject(parent),
      m_contentType(ContentType::Unknown),
      m_handler(ContentHandler::Source),
      m_complete(false)
{
    qDebug() << Q_FUNC_INFO;
    m_hub = cuc::Hub::Client::instance();
}

/*!
 * \brief \reimp
 * \internal
 */
void ContentPeerModel::classBegin()
{

}

/*!
 * \brief \reimp
 * \internal
 */
void ContentPeerModel::componentComplete()
{
    m_complete = true;
    QTimer::singleShot(0, this, SLOT(findPeers()));
}

/*!
 * \qmlproperty int ContentPeerModel::contentType
 *
 * Returns the ContentType 
 */
ContentType::Type ContentPeerModel::contentType()
{
    qDebug() << Q_FUNC_INFO;
    return m_contentType;
}

/*!
 * \brief ContentPeerModel::setContentType
 * \internal
 */
void ContentPeerModel::setContentType(ContentType::Type contentType)
{
    qDebug() << Q_FUNC_INFO;
    m_contentType = contentType;
    if (m_complete) {
        findPeers();
    }
    Q_EMIT contentTypeChanged();
}

/*!
 * \brief ContentPeerModel::findPeers
 * \internal
 */
void ContentPeerModel::findPeers() {
    qDebug() << Q_FUNC_INFO;
    m_peers.clear();
    QCoreApplication::processEvents();
    if(m_contentType == ContentType::All) {
        appendPeersForContentType(ContentType::Unknown);
        appendPeersForContentType(ContentType::Documents);
        appendPeersForContentType(ContentType::Pictures);
        appendPeersForContentType(ContentType::Music);
        appendPeersForContentType(ContentType::Contacts);
    } else {
        appendPeersForContentType(m_contentType);
    }
    Q_EMIT findPeersCompleted();
}

/*!
 * \brief ContentPeerModel::appendPeersForContentType
 * \internal
 */
void ContentPeerModel::appendPeersForContentType(ContentType::Type contentType)
{
    qDebug() << Q_FUNC_INFO;
    const cuc::Type &hubType = ContentType::contentType2HubType(contentType);
    QVector<cuc::Peer> hubPeers;
    if (m_handler == ContentHandler::Destination) {
        hubPeers = m_hub->known_destinations_for_type(hubType);
    } else if (m_handler == ContentHandler::Share) {
        hubPeers = m_hub->known_shares_for_type(hubType);
    } else {
        hubPeers = m_hub->known_sources_for_type(hubType);
    }

    Q_FOREACH (const cuc::Peer &hubPeer, hubPeers) 
    {
        if(!hubPeer.id().isEmpty()) 
        {
            ContentPeer *qmlPeer = new ContentPeer();
            qmlPeer->setPeer(hubPeer);
            qmlPeer->setContentType(contentType);
            qmlPeer->setHandler(m_handler);
            if(qmlPeer->isDefaultPeer()) 
            {
                m_peers.prepend(qmlPeer);
            } else {
                m_peers.append(qmlPeer);
            }
            Q_EMIT peersChanged();
        }
        QCoreApplication::processEvents();
    }
}

/*!
 * \qmlproperty int ContentPeerModel::handler
 *
 * Returns the ContentHandler 
 */
ContentHandler::Handler ContentPeerModel::handler() 
{
    qDebug() << Q_FUNC_INFO;
    return m_handler;
}

/*!
 * \brief ContentPeerModel::setHandler
 * \internal
 */
void ContentPeerModel::setHandler(ContentHandler::Handler handler)
{
    qDebug() << Q_FUNC_INFO;
    m_handler = handler;
    if (m_complete) {
        findPeers();
    }
    Q_EMIT handlerChanged();
}

QQmlListProperty<ContentPeer> ContentPeerModel::peers()
{
    qDebug() << Q_FUNC_INFO;
    return QQmlListProperty<ContentPeer>(this, m_peers);
}

