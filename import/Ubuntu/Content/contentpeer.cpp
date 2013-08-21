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

#include <QDebug>

/*!
 * \qmltype ContentPeer
 * \instantiates ContentPeer
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief FIXME add documentation
 *
 * FIXME add documentation
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

ContentPeer::ContentPeer(QObject *parent)
    : QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty string ContentPeer::name
 *
 * FIXME add documentation
 */
const QString &ContentPeer::name() const
{
    qDebug() << Q_FUNC_INFO;
    return m_name;
}

void ContentPeer::setName(const QString &name)
{
    qDebug() << Q_FUNC_INFO;
    if (name == m_name)
        return;

    m_name = name;
    Q_EMIT nameChanged();
}

/*!
 * \qmlproperty string ContentPeer::type
 *
 * FIXME add documentation
 */
const QString &ContentPeer::type() const
{
    qDebug() << Q_FUNC_INFO;
    return m_type;
}

void ContentPeer::setType(const QString &type)
{
    qDebug() << Q_FUNC_INFO;
    if (type == m_type)
        return;

    m_type = type;
    Q_EMIT typeChanged();
}
