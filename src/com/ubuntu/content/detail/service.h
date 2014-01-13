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
#ifndef SERVICE_H_
#define SERVICE_H_

#include <QObject>
#include <QStringList>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusContext>

#include <com/ubuntu/applicationmanager/application_manager.h>
#include "import_export_handler.h"

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{
class PeerRegistry;

class Service : public QObject, protected QDBusContext
{
    Q_OBJECT
  public:
    Service(QDBusConnection connection,
            const QSharedPointer<PeerRegistry>& registry,
            QSharedPointer<com::ubuntu::ApplicationManager::ApplicationManager>& application_manager,
            QObject* parent = nullptr);
    Service(const Service&) = delete;
    ~Service();

    Service& operator=(const Service&) = delete;

  public Q_SLOTS:
    QString DefaultPeerForType(const QString &type_id);
    QStringList KnownPeersForType(const QString &type_id);
    QDBusObjectPath CreateImportForTypeFromPeer(const QString&, const QString&, const QString&);
    void RegisterImportExportHandler(const QString&, const QString&, const QDBusObjectPath& handler);
    void RegisterShareHandler(const QString&, const QString&, const QDBusObjectPath& handler);
    void Quit();

  private:
    struct Private;
    QDBusServiceWatcher* m_watcher;
    QScopedPointer<Private> d;
    void connect_export_handler(const QString&, const QString&);
    void connect_import_handler(const QString&, const QString&);

  private Q_SLOTS:
    void handle_transfer(int);
    void handler_registered(const QString&);

};
}
}
}
}

#endif // SERVICE_H_
