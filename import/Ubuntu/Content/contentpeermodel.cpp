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

#include "../../../src/com/ubuntu/content/debug.h"
#include "contentpeermodel.h"
#include <stdio.h>

namespace cuc = com::ubuntu::content;

/*!
 * \qmltype ContentPeerModel
 * \instantiates ContentPeerModel
 * \inqmlmodule Ubuntu.Content
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
    TRACE() << Q_FUNC_INFO;
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
    findPeers();
}

/*!
 * \qmlproperty ContentType ContentPeerModel::contentType
 *
 * Specifies which ContentType discovered peers should support.
 */
ContentType::Type ContentPeerModel::contentType()
{
    TRACE() << Q_FUNC_INFO;
    return m_contentType;
}

/*!
 * \brief ContentPeerModel::setContentType
 * \internal
 */
void ContentPeerModel::setContentType(ContentType::Type contentType)
{
    TRACE() << Q_FUNC_INFO;
    if (m_contentType != contentType) {
        m_contentType = contentType;
        if (m_complete) {
            findPeers();
        }
        Q_EMIT contentTypeChanged();
    }
}

/*!
 * \brief ContentPeerModel::findPeers
 * \internal
 */
void ContentPeerModel::findPeers() {
    TRACE() << Q_FUNC_INFO;
    m_peers.clear();
    if(m_contentType == ContentType::All) {
        appendPeersForContentType(ContentType::Documents);
        appendPeersForContentType(ContentType::Pictures);
        appendPeersForContentType(ContentType::Music);
        appendPeersForContentType(ContentType::Contacts);
        appendPeersForContentType(ContentType::Videos);
        appendPeersForContentType(ContentType::Links);
        appendPeersForContentType(ContentType::EBooks);
        appendPeersForContentType(ContentType::Text);
        appendPeersForContentType(ContentType::Events);
        appendPeersForContentType(ContentType::Unknown);
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
    TRACE() << Q_FUNC_INFO;
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
            bool isDefault = false;
            bool isDupe = false;
            Q_FOREACH (ContentPeer *p, m_peers)
            {
                if (p->peer().id() == hubPeer.id())
                    isDupe = true;
            }
            if (isDupe)
                continue;

            ContentPeer *qmlPeer = new ContentPeer();
            qmlPeer->setPeer(hubPeer);
            qmlPeer->setHandler(m_handler);
            if(m_contentType != ContentType::All) {
                qmlPeer->setContentType(contentType);
                isDefault = qmlPeer->isDefaultPeer();
            } else {
                qmlPeer->setContentType(m_contentType);
            }

            if(isDefault)
                m_peers.prepend(qmlPeer);
            else
                m_peers.append(qmlPeer);
            
            Q_EMIT peersChanged();
        }
    }
}

/*!
 * \qmlproperty ContentHandler ContentPeerModel::handler
 *
 * Specifies which ContentHandler discovered peers should support.
 */
ContentHandler::Handler ContentPeerModel::handler() 
{
    TRACE() << Q_FUNC_INFO;
    return m_handler;
}

/*!
 * \brief ContentPeerModel::setHandler
 * \internal
 */
void ContentPeerModel::setHandler(ContentHandler::Handler handler)
{
    TRACE() << Q_FUNC_INFO;
    if (m_handler != handler) {
        m_handler = handler;
        if (m_complete) {
            findPeers();
        }
        Q_EMIT handlerChanged();
    }
}

/*!
 * \qmlproperty list<ContentPeer> ContentPeerModel::peers
 *
 * Provides a list of discovered peers matching the requested ContentType and ContentHandler.
 */
QQmlListProperty<ContentPeer> ContentPeerModel::peers()
{
    TRACE() << Q_FUNC_INFO;
    return QQmlListProperty<ContentPeer>(this, m_peers);
}

