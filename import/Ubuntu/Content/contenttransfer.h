/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COM_UBUNTU_CONTENTTRANSFER_H_
#define COM_UBUNTU_CONTENTTRANSFER_H_

#include <com/ubuntu/content/transfer.h>

#include <QList>
#include <QObject>
#include <QQmlListProperty>

namespace com {
namespace ubuntu {
namespace content {
class Transfer;
}
}
}

class ContentItem;

class ContentTransfer : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QQmlListProperty<ContentItem> items READ items NOTIFY itemsChanged)

public:
    enum State {
        Initiated = com::ubuntu::content::Transfer::initiated,
        InProgress = com::ubuntu::content::Transfer::in_progress,
        Charged = com::ubuntu::content::Transfer::charged,
        Collected = com::ubuntu::content::Transfer::collected,
        Aborted = com::ubuntu::content::Transfer::aborted
    };

    ContentTransfer(QObject *parent = nullptr);

    State state() const;
    void setState(State state);

    QQmlListProperty<ContentItem> items();

    Q_INVOKABLE bool start();

    com::ubuntu::content::Transfer *transfer() const;
    void setTransfer(com::ubuntu::content::Transfer *transfer);

Q_SIGNALS:
    void stateChanged();
    void itemsChanged();

private:
    void collectItems();

    com::ubuntu::content::Transfer *m_transfer;
    QList<ContentItem *> m_items;
};

#endif // COM_UBUNTU_CONTENTTRANSFER_H_
