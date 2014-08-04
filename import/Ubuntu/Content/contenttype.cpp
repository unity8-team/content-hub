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
#include "../../../src/com/ubuntu/content/debug.h"

/*!
   \qmltype ContentType
   \instantiates ContentType
   \inqmlmodule Ubuntu.Content
   \brief Describes the type of content to transfer

   \sa ContentHub

   \e {ContentType} is an enumeration of well known content types:
   \table
   \header
     \li Type
     \li Description
   \row
     \li ContentType.Uknown
     \li Unknown type
   \row
     \li ContentType.Documents
     \li Documents
   \row
     \li ContentType.Pictures
     \li Pictures
   \row
     \li ContentType.Music
     \li Music
   \row
     \li ContentType.Contacts
     \li Contacts
   \row
     \li ContentType.Videos
     \li Videos
   \row
     \li ContentType.Links
     \li Links
   \row
     \li ContentType.All
     \li Any of the above content types
   \endtable
 */

namespace cuc = com::ubuntu::content;


ContentType::ContentType(QObject *parent)
    : QObject(parent)
{
    TRACE() << Q_FUNC_INFO;
}


/*!
 * \brief ContentType::contentType2HubType converts a ContentType::Type to a
 * com::ubuntu::content::Type
 * \internal
 */
const com::ubuntu::content::Type &ContentType::contentType2HubType(int type)
{
    Type ctype = static_cast<Type>(type);
    TRACE() << Q_FUNC_INFO << ctype;
    return contentType2HubType(ctype);
}

/*!
 * \brief ContentType::contentType2HubType converts a ContentType::Type to a
 * com::ubuntu::content::Type
 * \internal
 */
const com::ubuntu::content::Type &ContentType::contentType2HubType(Type type)
{
    switch(type) {
    case Documents: return cuc::Type::Known::documents();
    case Pictures: return cuc::Type::Known::pictures();
    case Music: return cuc::Type::Known::music();
    case Contacts: return cuc::Type::Known::contacts();
    case Videos: return cuc::Type::Known::videos();
    case Links: return cuc::Type::Known::links();
    default: return cuc::Type::unknown();
    }
}

/*!
 * \brief ContentType::hubType2contentType converts a 
 * com::ubuntu::content::Type to a ContentType::Type
 * \internal
 */
ContentType::Type ContentType::hubType2contentType(const QString& type)
{
    if (type == cuc::Type::Known::documents().id())
        return Documents;
    else if (type ==  cuc::Type::Known::pictures().id())
        return Pictures;
    else if (type ==  cuc::Type::Known::music().id())
        return Music;
    else if (type ==  cuc::Type::Known::contacts().id())
        return Contacts;
    else if (type ==  cuc::Type::Known::videos().id())
        return Videos;
    else if (type ==  cuc::Type::Known::links().id())
        return Links;
    else
      return Unknown;
}
