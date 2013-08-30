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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */
#ifndef HANDLER_H_
#define HANDLER_H_

#include <QObject>
#include <QList>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusMessage>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include <com/ubuntu/content/import_export_handler.h>
#include <com/ubuntu/content/peer.h>

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
            const QString &peer_id,
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
    com::ubuntu::content::ImportExportHandler *m_handler;

  private Q_SLOTS:
    QList<QDBusObjectPath> get_transfers(const QString &peer_id, QString type);

};
}
}
}
}

#endif // HANDLER_H_
