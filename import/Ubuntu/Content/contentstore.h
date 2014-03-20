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

#ifndef COM_UBUNTU_CONTENTSTORE_H_
#define COM_UBUNTU_CONTENTSTORE_H_

#include "contentscope.h"
#include "contenttype.h"
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/store.h>

#include <QObject>
#include <QString>

class ContentStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uri READ uri NOTIFY uriChanged)
    Q_PROPERTY(ContentScope::Scope scope READ scope WRITE setScope NOTIFY scopeChanged)

public:
    ContentStore(QObject *parent = nullptr);

    const QString &uri() const;

    const com::ubuntu::content::Store *store() const;
    void setStore(const com::ubuntu::content::Store *store);

    ContentScope::Scope scope();
    void setScope(ContentScope::Scope scope);

    void updateStore(ContentType::Type type);

Q_SIGNALS:
    void uriChanged();
    void scopeChanged();

private:
    com::ubuntu::content::Hub *m_hub;
    const com::ubuntu::content::Store *m_store;
    ContentScope::Scope m_scope;
};

#endif // COM_UBUNTU_CONTENTSTORE_H_
