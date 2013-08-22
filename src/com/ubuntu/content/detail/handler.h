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
#ifndef HANDLER_H_
#define HANDLER_H_

#include <QObject>
#include <QStringList>
#include <com/ubuntu/content/import_export_handler.h>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{

class Handler : public QObject
{
    Q_OBJECT
  public:
    Handler(QDBusConnection connection,
            com::ubuntu::content::ImportExportHandler *handler = nullptr);
    Handler(const Handler&) = delete;
    ~Handler();

    Handler& operator=(const Handler&) = delete;

  public Q_SLOTS:
    void HandleImport(const QDBusObjectPath &transfer);
    void HandleExport(const QDBusObjectPath &transfer);

  private:
    struct Private;
    QScopedPointer<Private> d;
};
}
}
}
}

#endif // HANDLER_H_
