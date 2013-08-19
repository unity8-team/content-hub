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

#include <QCoreApplication>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/transfer.h>
#include <QDebug>
#include "example.h"

Example::Example(QObject *parent) :
    QObject(parent)
{
    m_hub = cuc::Hub::Client::instance();
    m_importer = new ExampleImporter();
    m_hub->register_import_export_handler(m_importer);
}

cuc::Transfer* Example::create_import()
{
    auto peer = m_hub->default_peer_for_type(cuc::Type::Known::pictures());
    qDebug() << "PEER: " << peer.id();

    cuc::Transfer *transfer = m_hub->create_import_for_type_from_peer(
        cuc::Type::Known::pictures(),
        peer);

    QObject::connect(transfer,
        SIGNAL(stateChanged()),
        this,
        SLOT (import()));

    return transfer;
}

void Example::import()
{
    cuc::Transfer *transfer = dynamic_cast<cuc::Transfer*>(sender());

    qDebug() << "STATE:" << transfer->state();
    if (transfer->state() == cuc::Transfer::charged)
        m_importer->handle_import(transfer);
}
