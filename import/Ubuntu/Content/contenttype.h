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

#ifndef COM_UBUNTU_CONTENTTYPE_H_
#define COM_UBUNTU_CONTENTTYPE_H_

#include <com/ubuntu/content/type.h>

#include <QObject>
#include <QString>

class ContentType : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)

public:
    /*!
      \qmlproperty Type Type
      \brief ContentType::Type well known ContentType
      \e {Type} is an enumeration:
      \list
        \li \b{Type.Unknown}: Unknown type
        \li \b{Type.Documents}: Documents
        \li \b{Type.Pictures}: Pictures
        \li \b{Type.Music}: Music
      \endlist
     */
    / *!
        \enum ContentType::Type

        This enum well known content types:

        \value Unknown
               The top-left corner of the rectangle.
        \value Documents
               The top-right corner of the rectangle.
        \value Pictures
               The bottom-left corner of the rectangle.
        \value Music
               The bottom-right corner of the rectangle.

    * /
    enum Type {
        Unknown = 0,
        Documents = 1,
        Pictures = 2,
        Music = 3
    };

    ContentType(QObject *parent = nullptr);

    static const com::ubuntu::content::Type &contentType2HubType(int type);
    static const com::ubuntu::content::Type &contentType2HubType(Type type);
};

#endif // COM_UBUNTU_CONTENTTYPE_H_
