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

#include "contentscope.h"

#include <QDebug>

/*!
   \qmltype ContentScope
   \instantiates ContentScope
   \inqmlmodule Ubuntu.Content 0.1

   \sa ContentStore

   \e {ContentScope} is an enumeration of scope types:
   \table
   \header
     \li Scope
   \row
     \li ContentScope.System
   \row
     \li ContentScope.User
   \row
     \li ContentScope.App
   \endtable
 */

ContentScope::ContentScope(QObject *parent)
    : QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \brief ContentScope::contentScope2HubScope converts a ContentScope::Scope to a
 * com::ubuntu::content::Scope
 * \internal
 */
com::ubuntu::content::Scope ContentScope::contentScope2HubScope(int scope)
{
    Scope cscope = static_cast<Scope>(scope);
    qDebug() << Q_FUNC_INFO << cscope;
    return contentScope2HubScope(cscope);
}

/*!
 * \brief ContentScope::contentScope2HubScope converts a ContentScope::Scope to a
 * com::ubuntu::content::Scope
 * \internal
 */
com::ubuntu::content::Scope ContentScope::contentScope2HubScope(Scope scope)
{
    return static_cast<com::ubuntu::content::Scope>(scope);
}

