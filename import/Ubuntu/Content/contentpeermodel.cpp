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
      m_type(0)
{
    qDebug() << Q_FUNC_INFO;
    m_hub = cuc::Hub::Client::instance();
}

/*!
 * \qmlproperty int ContentPeerModel::type
 *
 * Returns the ContentType 
 */
int ContentPeerModel::type()
{
    qDebug() << Q_FUNC_INFO;
    return m_type;
}

/*!
 * \brief ContentPeerModel::setType
 * \internal
 */
void ContentPeerModel::setType(int type)
{
    qDebug() << Q_FUNC_INFO;
    m_type = type;
    const cuc::Type &hubType = ContentType::contentType2HubType(type);
    QVector<cuc::Peer> hubPeers = m_hub->known_peers_for_type(hubType);

    Q_FOREACH (const cuc::Peer &hubPeer, hubPeers) {
        ContentPeer *qmlPeer = new ContentPeer();
        qmlPeer->setPeer(hubPeer);
        m_peers.append(QVariant::fromValue(qmlPeer));
    }
    Q_EMIT typeChanged();
}

/*!
 * \qmlproperty int ContentPeerModel::handler
 *
 * Returns the ContentHandler 
 */
int ContentPeerModel::handler() {
    qDebug() << Q_FUNC_INFO;
    return m_handler;
}

/*!
 * \brief ContentPeerModel::setHandler
 * \internal
 */
void ContentPeerModel::setHandler(int handler)
{
    qDebug() << Q_FUNC_INFO;
    m_handler = handler;

    Q_EMIT handlerChanged();
}

QVariantList ContentPeerModel::peers()
{
    qDebug() << Q_FUNC_INFO;
    return m_peers;
}

