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

#ifndef EXAMPLEIMPORTER_H
#define EXAMPLEIMPORTER_H
#include <QObject>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/transfer.h>
#include <com/ubuntu/content/import_export_handler.h>
#include <QDebug>
#include <QSharedPointer>

namespace cuc = com::ubuntu::content;

class ExampleImporter : public cuc::ImportExportHandler
{
    Q_OBJECT

public:
    ExampleImporter();
    Q_INVOKABLE void handle_import(cuc::Transfer*);
    Q_INVOKABLE void handle_export(cuc::Transfer*);    
};

#endif // EXAMPLEIMPORTER_H
