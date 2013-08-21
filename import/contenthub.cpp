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

ContentHub::ContentHub(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlmethod ContentHub::defaultSourceForType()
 *
 *  FIXME add documentation
 */
ContentPeer *ContentHub::defaultSourceForType(int type)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    return new ContentPeer(this);
}

/*!
 * \qmlmethod ContentHub::knownSourcesForType()
 *
 *  FIXME add documentation
 */
QList<ContentPeer *> ContentHub::knownSourcesForType(int type)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(type);
    QList<ContentPeer *> list;
    return list;
}

/*!
 * \qmlmethod ContentHub::importContent()
 *
 *  FIXME add documentation
 */
ContentTransfer *ContentHub::importContent(int type)
{
    qDebug() << Q_FUNC_INFO << (ContentType::Type)type;
    return new ContentTransfer(this);
}

/*!
 * \qmlmethod ContentHub::importContent()
 *
 *  FIXME add documentation
 */
ContentTransfer *ContentHub::importContent(int type, ContentPeer *peer)
{
    qDebug() << Q_FUNC_INFO << (ContentType::Type)type << peer;
    return new ContentTransfer(this);
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
