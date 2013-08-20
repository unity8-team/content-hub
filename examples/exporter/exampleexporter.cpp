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

#include "exampleexporter.h"

ExampleExporter::ExampleExporter()
{
    auto hub = cuc::Hub::Client::instance();
    hub->register_import_export_handler(this);
}

void ExampleExporter::handle_export(cuc::Transfer *transfer)
{
    QVector<cuc::Item> items;
    items << cuc::Item(QUrl("file:///tmp/test1"));
    items << cuc::Item(QUrl("file:///tmp/test2"));
    transfer->charge(items);
    qDebug() << Q_FUNC_INFO << "Items:" << transfer->collect().count();
}

void ExampleExporter::handle_import(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}
