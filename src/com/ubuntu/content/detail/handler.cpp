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

#include "handler.h"

#include "ContentTransferInterface.h"

#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/type.h>
#include <com/ubuntu/content/transfer.h>


#include <QCache>
#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include <QUuid>

#include <cassert>

namespace cucd = com::ubuntu::content::detail;
namespace cuc = com::ubuntu::content;

struct cucd::Handler::Private : public QObject
{
    Private(QDBusConnection connection,
            QObject* parent)
            : QObject(parent),
              connection(connection)
    {
        qDebug() << Q_FUNC_INFO;
    }

    QDBusConnection connection;
};

cucd::Handler::Handler(QDBusConnection connection, cuc::ImportExportHandler* handler)
        : d(new Private{connection, this})
{
    qDebug() << Q_FUNC_INFO;
    m_handler = handler;
}

cucd::Handler::~Handler() {}

void cucd::Handler::HandleImport(const QDBusObjectPath& transfer)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(transfer);
}

void cucd::Handler::HandleExport(const QDBusObjectPath& transfer)
{
    qDebug() << Q_FUNC_INFO;
    //cuc::dbus::Transfer* client = new cuc::dbus::Transfer("com.ubuntu.content.dbus.Service", transfer.path(), QDBusConnection::sessionBus(), 0);
    //cuc::Transfer *t = dynamic_cast<cuc::Transfer*>(client->connection().objectRegisteredAt(client->path()));

    //m_handler->handle_export(t);

    /*
    qDebug() << "Connection connected == "<< QDBusConnection::sessionBus().isConnected();
    QObject* obj = QDBusConnection::sessionBus().objectRegisteredAt(transfer.path());
    bool bret = ((obj)?(true):(false));
    qDebug() << "Agent object found === "<< bret;
    Q_UNUSED(transfer);
    */
    auto c = QDBusConnection::sessionBus();
    qDebug() << Q_FUNC_INFO << "Connection: " << c.lastError().message();
    qDebug() << "Connection connected == "<< c.isConnected();
    auto t = new cuc::dbus::Transfer("com.ubuntu.content.dbus.Service", transfer.path(), c, nullptr);
    QObject* obj = t->connection().objectRegisteredAt(transfer.path());

    bool res = ((obj)?(true):(false));
    qDebug() << "transfer object found === "<< res << "lastError:" << c.lastError().message();



}
