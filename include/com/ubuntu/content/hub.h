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
#ifndef COM_UBUNTU_CONTENT_HUB_H_
#define COM_UBUNTU_CONTENT_HUB_H_

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/type.h>

#include <QObject>
#include <QVector>

namespace com
{
namespace ubuntu
{
namespace content
{
class ImportExportHandler;
class ShareHandler;
class Store;
class Transfer;

class Hub : public QObject
{
    Q_OBJECT

  public:
    struct Client
    {
        static Hub* instance();
    };

    Hub(const Hub&) = delete;
    virtual ~Hub();
    Hub& operator=(const Hub&) = delete;

    Q_INVOKABLE virtual void register_import_export_handler(ImportExportHandler* handler);
    Q_INVOKABLE virtual void register_share_handler(ShareHandler* handler);
    Q_INVOKABLE virtual const Store* store_for_scope_and_type(Scope scope, Type type);
    Q_INVOKABLE virtual Peer default_peer_for_type(Type type);
    Q_INVOKABLE virtual QVector<Peer> known_peers_for_type(Type type);
    Q_INVOKABLE virtual Transfer* create_import_for_type_from_peer(Type type, Peer peer);

    Q_INVOKABLE virtual void quit();
       
  protected:
    Hub(QObject* = nullptr);
    
  private:
    struct Private;
    QScopedPointer<Private> d;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_HUB_H_
