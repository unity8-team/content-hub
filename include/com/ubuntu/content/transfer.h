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
#include <com/ubuntu/content/store.h>

#include <QObject>
#include <QSharedPointer>
#include <QVector>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{
class Handler;
}
}
}
}

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
    Q_ENUMS(SelectionType)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QVector<Item> items READ collect WRITE charge)
    Q_PROPERTY(Store store READ store NOTIFY storeChanged)
    Q_PROPERTY(SelectionType selectionType READ selectionType WRITE setSelectionType NOTIFY selectionTypeChanged)
    Q_PROPERTY(QVector<QString> types READ types WRITE setTypes NOTIFY typesChanged)

  public:
    enum State
    {
        created,
        initiated,
        in_progress,
        charged,
        collected,
        aborted,
        finalized
    };

    enum SelectionType
    {
        single,
        multiple
    };

    Transfer(const Transfer&) = delete;
    virtual ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

    Q_INVOKABLE virtual int id() const;
    Q_INVOKABLE virtual State state() const;
    Q_INVOKABLE virtual SelectionType selectionType() const;
    Q_INVOKABLE virtual bool start();
    Q_INVOKABLE virtual bool abort();
    Q_INVOKABLE virtual bool finalize();
    Q_INVOKABLE virtual bool charge(const QVector<Item>& items);
    Q_INVOKABLE virtual QVector<Item> collect();
    Q_INVOKABLE virtual Store store() const;
    Q_INVOKABLE virtual bool setStore(const Store*);
    Q_INVOKABLE virtual bool setSelectionType(const SelectionType&);
    Q_INVOKABLE virtual QVector<QString> types();
    Q_INVOKABLE virtual bool setTypes(const QVector<QString>& types);

    Q_SIGNAL void stateChanged();
    Q_SIGNAL void storeChanged();
    Q_SIGNAL void selectionTypeChanged();
    Q_SIGNAL void typesChanged();

  private:
    struct Private;
    friend struct Private;
    friend class Hub;
    friend class com::ubuntu::content::detail::Handler;
    QSharedPointer<Private> d;

    Transfer(const QSharedPointer<Private>&, QObject* parent = nullptr);    
};
}
}
}

#endif // COM_UBUNTU_CONTENT_TRANSFER_H_
