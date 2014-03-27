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
#include "contenthandler.h"

/*!
   \qmltype ContentHandler
   \instantiates ContentHandler
   \inqmlmodule Ubuntu.Content 0.1

   \sa ContentHub

   \e {ContentHandler} is an enumeration of handler types:
   \table
   \header
     \li Handler
   \row
     \li ContentHandler.Source
   \row
     \li ContentHandler.Destination
   \row
     \li ContentHandler.Share
   \endtable
 */

ContentHandler::ContentHandler(QObject *parent)
    : QObject(parent)
{
    TRACE() << Q_FUNC_INFO;
}
