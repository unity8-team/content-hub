/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */
#ifndef COM_UBUNTU_CONTENT_IMPORT_EXPORT_HANDLER_H_
#define COM_UBUNTU_CONTENT_IMPORT_EXPORT_HANDLER_H_

#include <QObject>

namespace com
{
namespace ubuntu
{
namespace content
{
class Transfer;

class ImportExportHandler : public QObject
{
    Q_OBJECT
  public:
    ImportExportHandler(const ImportExportHandler&) = delete;
    virtual ~ImportExportHandler() = default;
    ImportExportHandler& operator=(const ImportExportHandler&) = delete;

    Q_INVOKABLE virtual void handle_import(Transfer*) = 0;
    Q_INVOKABLE virtual void handle_export(Transfer*) = 0;

  protected:
    ImportExportHandler(QObject* = nullptr); 
};
}
}
}

#endif // COM_UBUNTU_CONTENT_IMPORT_EXPORT_HANDLER_H_
