/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include "example.h"

#include <com/ubuntu/content/store.h>

Example::Example(QObject *parent) :
    QObject(parent),
    m_importer(new ExampleImporter())
{
}

void Example::create_import()
{
    auto hub = cuc::Hub::Client::instance();

    auto peer = hub->default_peer_for_type(cuc::Type::Known::pictures());
    qDebug() << Q_FUNC_INFO << "PEER: " << peer.name();

    m_transfer = hub->create_import_for_type_from_peer(
        cuc::Type::Known::pictures(),
        peer);

    /* Uncommit this for persistent storage
    auto store = hub->store_for_scope_and_type(cuc::Scope::app, cuc::Type::Known::pictures());
    qDebug() << Q_FUNC_INFO << "STORE:" << store->uri();
    m_transfer->setStore(store);
    */

    QVector<QString> myTypes;
    myTypes << "image/png";
    myTypes << "image/jpeg";
    m_transfer->setTypes(myTypes);
    m_transfer->setSelectionType(cuc::Transfer::SelectionType::multiple);
    m_transfer->start();
}
