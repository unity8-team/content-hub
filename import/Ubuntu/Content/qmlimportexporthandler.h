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

#ifndef COM_UBUNTU_QMLIMPORTEXPORTHANDLER_H_
#define COM_UBUNTU_QMLIMPORTEXPORTHANDLER_H_

#include <com/ubuntu/content/import_export_handler.h>

namespace com {
namespace ubuntu {
namespace content {
class Transfer;
}
}
}

class QmlImportExportHandler : public com::ubuntu::content::ImportExportHandler
{
    Q_OBJECT

public:
    QmlImportExportHandler(QObject *parent = nullptr);

    Q_INVOKABLE virtual void handle_import(com::ubuntu::content::Transfer *transfer);
    Q_INVOKABLE virtual void handle_export(com::ubuntu::content::Transfer *transfer);

Q_SIGNALS:
    void importRequested(com::ubuntu::content::Transfer*);
    void exportRequested(com::ubuntu::content::Transfer*);

};

#endif // COM_UBUNTU_QMLIMPORTEXPORTHANDLER_H_
