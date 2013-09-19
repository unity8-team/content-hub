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

class ContentItem;

class ContentTransfer : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(Direction)
    Q_ENUMS(SelectionType)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(Direction direction READ direction CONSTANT)
    Q_PROPERTY(SelectionType selectionType READ selectionType WRITE setSelectionType NOTIFY selectionTypeChanged)
    Q_PROPERTY(QQmlListProperty<ContentItem> items READ items NOTIFY itemsChanged)

public:
    enum State {
        Created = com::ubuntu::content::Transfer::created,
        Initiated = com::ubuntu::content::Transfer::initiated,
        InProgress = com::ubuntu::content::Transfer::in_progress,
        Charged = com::ubuntu::content::Transfer::charged,
        Collected = com::ubuntu::content::Transfer::collected,
        Aborted = com::ubuntu::content::Transfer::aborted,
        Finalized = com::ubuntu::content::Transfer::finalized
    };
    enum Direction {
        Import,
        Export
    };
    enum SelectionType {
        Single = com::ubuntu::content::Transfer::SelectionType::single,
        Multiple = com::ubuntu::content::Transfer::SelectionType::multiple
    };

    ContentTransfer(QObject *parent = nullptr);

    State state() const;
    void setState(State state);

    Direction direction() const;

    SelectionType selectionType() const;
    void setSelectionType(SelectionType);

    QQmlListProperty<ContentItem> items();

    Q_INVOKABLE bool start();
    Q_INVOKABLE bool finalize();

    com::ubuntu::content::Transfer *transfer() const;
    void setTransfer(com::ubuntu::content::Transfer *transfer, Direction direction);

    void collectItems();

Q_SIGNALS:
    void stateChanged();
    void itemsChanged();
    void selectionTypeChanged();

private Q_SLOTS:
    void updateState();
    void updateSelectionType();

private:
    com::ubuntu::content::Transfer *m_transfer;
    QList<ContentItem *> m_items;
    State m_state;
    Direction m_direction;
    SelectionType m_selectionType;
};

#endif // COM_UBUNTU_CONTENTTRANSFER_H_
