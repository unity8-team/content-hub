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

#include "contentpeer.h"

#include <com/ubuntu/content/peer.h>

#include <QDebug>

/*!
 * \qmltype ContentPeer
 * \instantiates ContentPeer
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief FIXME add documentation
 *
 * FIXME add documentation
 *
 * See documentation for \ContentHub
 */

namespace cuc = com::ubuntu::content;

ContentPeer::ContentPeer(QObject *parent)
    : QObject(parent),
      m_peer(0)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty string ContentPeer::name
 *
 * FIXME add documentation
 */
QString ContentPeer::name()
{
    qDebug() << Q_FUNC_INFO;
    return m_peer.name();
}

/*!
 * \qmlproperty string ContentPeer::id
 *
 * FIXME add documentation
 */
const QString &ContentPeer::id() const
{
    qDebug() << Q_FUNC_INFO;
    return m_peer.id();
}

/*!
 * \qmlproperty string ContentPeer::type
 *
 * FIXME add documentation
 */
const QString &ContentPeer::type() const
{
    qDebug() << Q_FUNC_INFO;
    // FIXME return the type
    return m_peer.id();
}

/*!
 * \brief ContentPeer::peer
 * \return
 */
const com::ubuntu::content::Peer &ContentPeer::peer() const
{
    return m_peer;
}

/*!
 * \brief ContentPeer::setPeer
 * \param peer
 */
void ContentPeer::setPeer(const cuc::Peer &peer)
{
    m_peer = peer;
    Q_EMIT nameChanged();
    Q_EMIT typeChanged();
}
