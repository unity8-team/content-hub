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

namespace cuc = com::ubuntu::content;

ContentType::ContentType(QObject *parent)
    : QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \brief ContentType::contentType2HubType converts a ContentType::Type to a
 * com::ubuntu::content::Type
 * \param type integer representing a ContentType::Type
 * \return
 */
const com::ubuntu::content::Type &ContentType::contentType2HubType(int type)
{
    Type ctype = static_cast<Type>(type);
    qDebug() << Q_FUNC_INFO << ctype;
    return contentType2HubType(ctype);
}

/*!
 * \brief ContentType::contentType2HubType converts a ContentType::Type to a
 * com::ubuntu::content::Type
 * \param type type of the content
 * \return
 */
const com::ubuntu::content::Type &ContentType::contentType2HubType(Type type)
{
    switch(type) {
    case Documents: return cuc::Type::Known::documents();
    case Pictures: return cuc::Type::Known::pictures();
    case Music: return cuc::Type::Known::music();
    default: return cuc::Type::unknown();
    }
}
