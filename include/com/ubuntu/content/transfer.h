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
#ifndef COM_UBUNTU_CONTENT_TRANSFER_H_
#define COM_UBUNTU_CONTENT_TRANSFER_H_

#include <com/ubuntu/content/item.h>

#include <QObject>
#include <QSharedPointer>
#include <QVector>

namespace com
{
namespace ubuntu
{
namespace content
{
class Item;

class Transfer : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QVector<Item> items READ collect WRITE charge)

  public:
    enum State
    {
        initiated,
        in_progress,
        charged,
        collected,
        aborted
    };

    Transfer(const Transfer&) = delete;
    virtual ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

    Q_INVOKABLE virtual State state() const;

    Q_INVOKABLE virtual bool start();
    Q_INVOKABLE virtual bool abort();
    Q_INVOKABLE virtual bool charge(const QVector<Item>& items);
    Q_INVOKABLE virtual QVector<Item> collect();

    Q_SIGNAL void stateChanged();

  private:
    struct Private;
    friend struct Private;
    friend class Hub;
    QSharedPointer<Private> d;

    Transfer(const QSharedPointer<Private>&, QObject* parent = nullptr);    
};
}
}
}

#endif // COM_UBUNTU_CONTENT_TRANSFER_H_
