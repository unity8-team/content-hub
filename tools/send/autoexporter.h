/*
 * Copyright (C) 2015 Canonical, Ltd.
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

#ifndef AUTOEXPORTER_H
#define AUTOEXPORTER_H

#include <QObject>
#include <QString>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/transfer.h>
#include <com/ubuntu/content/import_export_handler.h>

namespace cuc = com::ubuntu::content;

class AutoExporter : public cuc::ImportExportHandler
{
    Q_OBJECT

public:
    AutoExporter();
       
public slots:
    Q_INVOKABLE void handle_import(cuc::Transfer*);
    Q_INVOKABLE void handle_export(cuc::Transfer*);
    Q_INVOKABLE void handle_share(cuc::Transfer*);
    Q_INVOKABLE void stateChanged();
    void setUrl(const QString&);
    void setText(const QString&);

private:
    QString url;
    QString text;
};

#endif // AUTOEXPORTER_H
