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

#include "examplesharehandler.h"

ExampleShareHandler::ExampleShareHandler()
{
    auto hub = cuc::Hub::Client::instance();
    hub->register_share_handler(this);
}

void ExampleShareHandler::handle_share(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO;
    if (transfer == nullptr)
    {
        qDebug() << Q_FUNC_INFO << "Transfer null";
        return;
    }

    if (transfer->selectionType() == cuc::Transfer::SelectionType::single)
        qDebug() << Q_FUNC_INFO << "selectionType: single";
    else if (transfer->selectionType() == cuc::Transfer::SelectionType::multiple)
        qDebug() << Q_FUNC_INFO << "selectionType: multiple";

    QVector<cuc::Item> items;
    items = transfer->collect();
    qDebug() << Q_FUNC_INFO << "Items:" << items.count();
}
