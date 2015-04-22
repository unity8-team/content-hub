/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#ifndef AUTOSHARER_H
#define AUTOSHARER_H

#include <QObject>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/transfer.h>
#include <com/ubuntu/content/type.h>
#include <com/ubuntu/content/import_export_handler.h>
#include <QDebug>

namespace cuc = com::ubuntu::content;

class AutoSharer : public cuc::ImportExportHandler
{
    Q_OBJECT

public:
    AutoSharer();
       
public slots:
    Q_INVOKABLE void handle_import(cuc::Transfer*);
    Q_INVOKABLE void handle_export(cuc::Transfer*);
    Q_INVOKABLE void handle_share(cuc::Transfer*);
    Q_INVOKABLE void stateChanged();
};

#endif // AUTOSHARER_H
