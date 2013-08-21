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

#include "contenttype.h"

#include <QDebug>

/*!
 * \qmltype ContentType
 * \instantiates ContentType
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

ContentType::ContentType(QObject *parent)
    : QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
}
