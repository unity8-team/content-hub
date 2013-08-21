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

#include <contentitem.h>

#include <com/ubuntu/content/transfer.h>

#include <QList>
#include <QObject>
#include <QQmlListProperty>

namespace cuc = com::ubuntu::content;

class ContentTransfer : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QQmlListProperty<ContentItem> items READ items)

public:
    enum State {
        Initiated = cuc::Transfer::initiated,
        InProgress = cuc::Transfer::in_progress,
        Charged = cuc::Transfer::charged,
        Collected = cuc::Transfer::collected,
        Aborted = cuc::Transfer::aborted
    };

    ContentTransfer(QObject *parent = nullptr);

    State state() const;

    QQmlListProperty<ContentItem> items();

    Q_INVOKABLE bool start();

Q_SIGNALS:
    void stateChanged();

private:
    QList<ContentItem *> m_items;
};

#endif // COM_UBUNTU_CONTENTTRANSFER_H_
