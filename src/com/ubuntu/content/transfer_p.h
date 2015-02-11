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
#ifndef COM_UBUNTU_CONTENT_TRANSFER_P_H_
#define COM_UBUNTU_CONTENT_TRANSFER_P_H_

#include "common.h"
#include "ContentTransferInterface.h"

#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/transfer.h>

#include <QObject>
#include <QVector>

namespace com
{
namespace ubuntu
{
namespace content
{
class Transfer::Private : public QObject
{
    Q_OBJECT
  public:
    static Transfer* make_transfer(const QDBusObjectPath& transfer, QObject* parent)
    {
        QSharedPointer<Private> d{new Private{transfer, parent}};
        return new Transfer{d, parent};
    }

    Private(const QDBusObjectPath& transfer, QObject* parent)
            : QObject(parent),
              remote_transfer(
                  new com::ubuntu::content::dbus::Transfer(
                      HUB_SERVICE_NAME,
                      transfer.path(), 
                      QDBusConnection::sessionBus(), this))
    {
    }

    int id()
    {
        auto reply = remote_transfer->Id();
        reply.waitForFinished();

        return reply.value();
    }

    State state()
    {
        auto reply = remote_transfer->State();
        reply.waitForFinished();

        if (reply.isError())
            return Transfer::aborted;

        return static_cast<Transfer::State>(reply.value());
    }

    bool start()
    {
        auto reply = remote_transfer->Start();
        reply.waitForFinished();
        
        return not reply.isError();
    }

    bool handled()
    {
        auto reply = remote_transfer->Handled();
        reply.waitForFinished();

        return not reply.isError();
    }

    bool abort()
    {
        auto reply = remote_transfer->Abort();
        reply.waitForFinished();
        
        return not reply.isError();
    }

    bool finalize()
    {
        auto reply = remote_transfer->Finalize();
        reply.waitForFinished();

        return not reply.isError();
    }

    bool charge(const QVector<Item>& items)
    {
        QVariantList itemVariants;
        Q_FOREACH(const Item& item, items)
        {   
            itemVariants << QVariant::fromValue(item);
        }
        auto reply = remote_transfer->Charge(itemVariants);
        reply.waitForFinished();

        return not reply.isError();
    }

    QVector<Item> collect()
    {
        QVector<Item> result;

        auto reply = remote_transfer->Collect();
        reply.waitForFinished();
        
        if (reply.isError())
            return result;

        auto items = reply.value();

        Q_FOREACH(const QVariant& itemVariant, items)
        {   
            result << qdbus_cast<Item>(itemVariant);
        }

        return result;
    }

    Store store()
    {
        auto reply = remote_transfer->Store();
        reply.waitForFinished();

        /* FIXME: Return something if it fails */
        /*
        if (reply.isError())
            return new cuc::Store{""};
        */

        //return new Store{reply.value()};
        return static_cast<Store>(reply.value());
    }

    bool setStore(const Store* store)
    {
        auto reply = remote_transfer->SetStore(store->uri());
        reply.waitForFinished();

        return not reply.isError();
    }

    SelectionType selection_type()
    {
        auto reply = remote_transfer->SelectionType();
        reply.waitForFinished();

        /* if SelectionType fails, default to single */
        if (reply.isError())
            return Transfer::SelectionType::single;

        return static_cast<Transfer::SelectionType>(reply.value());
    }

    bool setSelectionType(int type)
    {
        auto reply = remote_transfer->SetSelectionType(type);
        reply.waitForFinished();

        return not reply.isError();
    }

    Direction direction()
    {
        auto reply = remote_transfer->Direction();
        reply.waitForFinished();

        /* if Direction fails, default to import */
        if (reply.isError())
            return Transfer::Direction::Import;

        return static_cast<Transfer::Direction>(reply.value());
    }

    QString downloadId()
    {
        auto reply = remote_transfer->DownloadId();
        reply.waitForFinished();

        if (reply.isError())
            return QString();

        return static_cast<QString>(reply.value());
    }

    bool setDownloadId(QString downloadId)
    {
        auto reply = remote_transfer->SetDownloadId(downloadId);
        reply.waitForFinished();

        return not reply.isError();
    }

    bool download()
    {
        auto reply = remote_transfer->Download();
        reply.waitForFinished();

        return not reply.isError();
    }

    QString contentType()
    {
        auto reply = remote_transfer->ContentType();
        reply.waitForFinished();

        return static_cast<QString>(reply.value());
    }

    QString source()
    {
        auto reply = remote_transfer->source();
        reply.waitForFinished();

        return static_cast<QString>(reply.value());
    }

    QString destination()
    {
        auto reply = remote_transfer->destination();
        reply.waitForFinished();

        return static_cast<QString>(reply.value());
    }

    QString mirSocket()
    {
        auto reply = remote_transfer->MirSocket();
        reply.waitForFinished();

        return static_cast<QString>(reply.value());
    }

    bool setMirSocket(QString mir_socket)
    {
        auto reply = remote_transfer->SetMirSocket(mir_socket);
        reply.waitForFinished();

        return not reply.isError();
    }

    com::ubuntu::content::dbus::Transfer* remote_transfer;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_TRANSFER_P_H_
