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

#include "contentstore.h"

#include <QDebug>

/*!
 * \qmltype ContentStore
 * \instantiates ContentStore
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief FIXME add documentation
 *
 * FIXME add documentation
 *
 * See documentation for \ContentHub
 */

namespace cuc = com::ubuntu::content;

ContentStore::ContentStore(QObject *parent)
    : QObject(parent),
      m_store(0)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty uri ContentStore::uri
 *
 * URI of the content store
 */
const QString &ContentStore::uri() const
{
    qDebug() << Q_FUNC_INFO;
    return m_store->uri();
}

/*!
 * \brief ContentStore::store
 * \return
 */
const com::ubuntu::content::Store *ContentStore::store() const
{
    qDebug() << Q_FUNC_INFO;
    return m_store;
}

/*!
 * \brief ContentStore::setStore
 * \param store
 */
void ContentStore::setStore(const com::ubuntu::content::Store *store)
{
    qDebug() << Q_FUNC_INFO;
    //if (store == m_store)
    //    return;

    m_store = store;
    Q_EMIT uriChanged();
}
