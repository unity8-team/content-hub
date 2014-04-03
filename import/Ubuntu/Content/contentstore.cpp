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
#include "contentpeer.h"
#include "contentstore.h"
#include "contenttype.h"

/*!
 * \qmltype ContentStore
 * \instantiates ContentStore
 * \inqmlmodule Ubuntu.Content
 *
 * A ContentStore allows for the permanent storage of a transfered item.
 *
 * See documentation for ContentHub and ContentScope
 */

namespace cuc = com::ubuntu::content;

ContentStore::ContentStore(QObject *parent)
    : QObject(parent),
      m_store(0),
      m_scope(ContentScope::System)
{
    TRACE() << Q_FUNC_INFO;
    m_hub = cuc::Hub::Client::instance();
}

/*!
 * \qmlproperty uri ContentStore::uri
 *
 * URI of the content store
 */
const QString &ContentStore::uri() const
{
    static const QString __empty;

    TRACE() << Q_FUNC_INFO;

    if ( ! m_store) {
        qWarning() << "Accessing ContentStore uri with NULL internal store";
        return __empty;
    }
    return m_store->uri();
}

/*!
 * \brief ContentStore::store
 * \internal
 */
const com::ubuntu::content::Store *ContentStore::store() const
{
    TRACE() << Q_FUNC_INFO;
    return m_store;
}

/*!
 * \brief ContentStore::setStore
 * \internal
 */
void ContentStore::setStore(const com::ubuntu::content::Store *store)
{
    TRACE() << Q_FUNC_INFO;
    m_store = store;
    Q_EMIT uriChanged();
}

/*!
 * \qmlproperty ContentScope ContentStore::scope
 *
 * Specifies the ContentScope for this store.
 */
ContentScope::Scope ContentStore::scope()
{   
    TRACE() << Q_FUNC_INFO;
    return m_scope;
}

/*!
 * \brief ContentStore::setScope
 * \internal
 */
void ContentStore::setScope(ContentScope::Scope scope)
{   
    TRACE() << Q_FUNC_INFO;
    m_scope = scope;

    Q_EMIT scopeChanged();
}

/*!
 * \brief ContentStore::updateStore
 * \internal
 */
void ContentStore::updateStore(ContentType::Type contentType)
{
    TRACE() << Q_FUNC_INFO;

    com::ubuntu::content::Scope hubScope = ContentScope::contentScope2HubScope(m_scope);
    const com::ubuntu::content::Type &hubType = ContentType::contentType2HubType(contentType);
    setStore(m_hub->store_for_scope_and_type(hubScope, hubType));
}

