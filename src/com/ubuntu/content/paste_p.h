/*
 * Copyright © 2016 Canonical Ltd.
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
#ifndef COM_UBUNTU_CONTENT_PASTE_P_H_
#define COM_UBUNTU_CONTENT_PASTE_P_H_

#include "common.h"
#include "ContentPasteInterface.h"

#include <com/ubuntu/content/item.h>
#include <com/ubuntu/content/paste.h>

#include <QObject>
#include <QVector>

namespace com
{
namespace ubuntu
{
namespace content
{
class Paste::Private : public QObject
{
    Q_OBJECT
  public:
    static Paste* make_paste(const QDBusObjectPath& paste, QObject* parent)
    {
        QSharedPointer<Private> d{new Private{paste, parent}};
        return new Paste{d, parent};
    }

    Private(const QDBusObjectPath& paste, QObject* parent)
            : QObject(parent),
              remote_paste(
                  new com::ubuntu::content::dbus::Paste(
                      HUB_SERVICE_NAME,
                      paste.path(), 
                      QDBusConnection::sessionBus(), this))
    {
    }

    int id()
    {
        auto reply = remote_paste->Id();
        reply.waitForFinished();

        return reply.value();
    }

    State state()
    {
        auto reply = remote_paste->State();
        reply.waitForFinished();

        if (reply.isError())
            return Paste::aborted;

        return static_cast<Paste::State>(reply.value());
    }

    bool abort()
    {
        auto reply = remote_paste->Abort();
        reply.waitForFinished();
        
        return not reply.isError();
    }

    bool finalize()
    {
        auto reply = remote_paste->Finalize();
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
        auto reply = remote_paste->Charge(itemVariants);
        reply.waitForFinished();

        return not reply.isError();
    }

    QVector<Item> collect()
    {
        QVector<Item> result;

        auto reply = remote_paste->Collect();
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

    QString source()
    {
        auto reply = remote_paste->source();
        reply.waitForFinished();

        return static_cast<QString>(reply.value());
    }

    com::ubuntu::content::dbus::Paste* remote_paste;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_PASTE_P_H_
