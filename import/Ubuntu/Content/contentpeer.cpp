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
#include "contenttype.h"

#include <com/ubuntu/content/peer.h>

#include <QDebug>

/*!
 * \qmltype ContentPeer
 * \instantiates ContentPeer
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief An application that can export or import a ContentType
 *
 * A ContentPeer is an application that is registered in the ContentHub as
 * a source or destination of a ContentType
 *
 * See documentation for ContentHub
 */

namespace cuc = com::ubuntu::content;

ContentPeer::ContentPeer(QObject *parent)
    : QObject(parent),
      m_peer(0)
{
    qDebug() << Q_FUNC_INFO;
    m_hub = cuc::Hub::Client::instance();
}

/*!
 * \qmlproperty string ContentPeer::name
 *
 * Returns user friendly name of the peer
 */
QString ContentPeer::name()
{
    qDebug() << Q_FUNC_INFO;
    return m_peer.name();
}

/*!
 * \qmlproperty string ContentPeer::appId
 *
 * Returns the Application id
 */
const QString &ContentPeer::appId() const
{
    qDebug() << Q_FUNC_INFO;
    return m_peer.id();
}

/*!
 * \brief ContentPeer::setAppId
 *
 * Sets the Application id
 */
void ContentPeer::setAppId(const QString& appId)
{
    qDebug() << Q_FUNC_INFO << appId;
    // FIXME: Not sure if it's a good idea to be able to change the peer
    this->setPeer(cuc::Peer{appId});
}

/*!
 * \brief ContentPeer::peer
 * \internal
 */
const com::ubuntu::content::Peer &ContentPeer::peer() const
{
    return m_peer;
}

/*!
 * \brief ContentPeer::setPeer
 * \internal
 */
void ContentPeer::setPeer(const cuc::Peer &peer)
{
    m_peer = peer;
    Q_EMIT nameChanged();
    Q_EMIT appIdChanged();
}

/*!
 * \qmlproperty int ContentPeer::handler
 *
 * Returns the ContentHandler 
 */
int ContentPeer::handler() {
    qDebug() << Q_FUNC_INFO;
    return m_handler;
}

/*!
 * \brief ContentPeer::setHandler
 * \internal
 */
void ContentPeer::setHandler(int handler)
{   
    qDebug() << Q_FUNC_INFO;
    m_handler = handler;

    Q_EMIT handlerChanged();
}

/*!
 * \qmlproperty int ContentPeer::contentType
 *
 * Returns the ContentType
 */
int ContentPeer::contentType() 
{
    qDebug() << Q_FUNC_INFO;
    return m_contentType;
}

/*!
 * \brief ContentPeer::setContentType
 * \internal
 */
void ContentPeer::setContentType(int contentType)
{   
    qDebug() << Q_FUNC_INFO;
    m_contentType = contentType;

    const cuc::Type &hubType = ContentType::contentType2HubType(m_contentType);
    setPeer(m_hub->default_source_for_type(hubType));

    Q_EMIT contentTypeChanged();
}

/*!
 * \qmlmethod ContentPeer::request()
 *
 * \brief Request to import data from this \a ContentPeer
 */
ContentTransfer *ContentPeer::request()
{   
    qDebug() << Q_FUNC_INFO;

    cuc::Transfer *hubTransfer = m_hub->create_import_from_peer(m_peer);
// FIXME update tests so this can be enabled
//    if (!hubTransfer)
//        return nullptr;

    ContentTransfer *qmlTransfer = new ContentTransfer(this);
    qmlTransfer->setTransfer(hubTransfer);
    qmlTransfer->start();
    return qmlTransfer;
}

