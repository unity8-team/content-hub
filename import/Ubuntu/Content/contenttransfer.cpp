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

#include "contenttransfer.h"
#include "contentitem.h"

#include <com/ubuntu/content/item.h>

#include <QDebug>

/*!
 * \qmltype ContentTransfer
 * \instantiates ContentTransfer
 * \inqmlmodule Ubuntu.Content 0.1
 * \brief FIXME add documentation
 *
 * FIXME add documentation
 *
 * See documentation for \ContentHub
 */

namespace cuc = com::ubuntu::content;

ContentTransfer::ContentTransfer(QObject *parent)
    : QObject(parent),
      m_transfer(0),
      m_state(Aborted),
      m_direction(Import),
      m_selectionType(Single),
      m_store(0)
{
    qDebug() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty string ContentTransfer::state
 *
 * FIXME add documentation
 */
ContentTransfer::State ContentTransfer::state() const
{
    qDebug() << Q_FUNC_INFO;
    return m_state;
}

void ContentTransfer::setState(ContentTransfer::State state)
{
    qDebug() << Q_FUNC_INFO;
    if (!m_transfer)
        return;

    if (state == Charged && m_state == InProgress && m_direction == Export) {
        QVector<cuc::Item> hubItems;
        hubItems.reserve(m_items.size());
        foreach (const ContentItem *citem, m_items) {
            hubItems.append(citem->item());
        }
        m_transfer->charge(hubItems);
    }
}

/*!
 * \brief ContentTransfer::direction indicates if this transferobject is used for
 * import or export transaction
 * \return
 */
ContentTransfer::Direction ContentTransfer::direction() const
{
    return m_direction;
}

/*!
 * \brief ContentTransfer::selectionType indicates if this transferobject is allows
 * single or multiple selection of items
 * \return
 */
ContentTransfer::SelectionType ContentTransfer::selectionType() const
{
    return m_selectionType;
}

void ContentTransfer::setSelectionType(ContentTransfer::SelectionType type)
{
    qDebug() << Q_FUNC_INFO << type;
    if (!m_transfer)
        return;

    if (m_state == Created && (m_selectionType != type)) {
        m_transfer->setSelectionType(static_cast<cuc::Transfer::SelectionType>(type));
        updateSelectionType();
    }
}

/*!
 * \qmlproperty list<ContentItem> ContentTransfer::items
 *
 * FIXME add documentation
 */
QQmlListProperty<ContentItem> ContentTransfer::items()
{
    qDebug() << Q_FUNC_INFO;
    if (m_state == Charged && m_direction == Import) {
        collectItems();
    }
    return QQmlListProperty<ContentItem>(this, m_items);
}

/*!
 * \qmlmethod ContentTransfer::start()
 *
 *  FIXME add documentation
 */
bool ContentTransfer::start()
{
    qDebug() << Q_FUNC_INFO;
    if (m_state == Created) {
        return m_transfer->start();
    } else {
        qWarning() << Q_FUNC_INFO << "Transfer can't be started";
        return false;
    }
}

/*!
 * \qmlmethod ContentTransfer::finalize()
 *
 *  FIXME add documentation
 */
bool ContentTransfer::finalize()
{
    qDebug() << Q_FUNC_INFO;
    return m_transfer->finalize();
}

/*!
 * \brief ContentTransfer::store
 * \return
 */
const QString ContentTransfer::store() const
{
    qDebug() << Q_FUNC_INFO;
    return m_transfer->store().uri();
}

void ContentTransfer::setStore(ContentStore* contentStore)
{
    qDebug() << Q_FUNC_INFO;

    if (!m_transfer)
    {
        qWarning() << Q_FUNC_INFO << "invalid transfer";
        return;
    }
    m_transfer->setStore(contentStore->store());
    updateStore();
}

/*!
 * \brief ContentTransfer::transfer
 * \return
 */
com::ubuntu::content::Transfer *ContentTransfer::transfer() const
{
    qDebug() << Q_FUNC_INFO;
    return m_transfer;
}

/*!
 * \brief ContentTransfer::setTransfer
 * \param transfer
 */
void ContentTransfer::setTransfer(com::ubuntu::content::Transfer *transfer, Direction direction)
{
    if (m_transfer) {
        qWarning() << Q_FUNC_INFO << "the transfer object was already set";
        return;
    }

    if (!transfer) {
        qWarning() << Q_FUNC_INFO << "No valid transfer object passed:" << transfer;
        return;
    }

    qDebug() << Q_FUNC_INFO;

    m_direction = direction;
    m_transfer = transfer;

    updateSelectionType();
    updateStore();
    updateState();

    if (m_state == Charged && m_direction == Import)
        collectItems();

    connect(m_transfer, SIGNAL(stateChanged()), this, SLOT(updateState()));
    connect(m_transfer, SIGNAL(selectionTypeChanged()), this, SLOT(updateSelectionType()));
    connect(m_transfer, SIGNAL(storeChanged()), this, SLOT(updateStore()));
}

/*!
 * \brief ContentTransfer::collectItems gets the items out of the transfer object
 */
void ContentTransfer::collectItems()
{
    qDebug() << Q_FUNC_INFO;
    if (m_state != Charged || m_direction != Import)
        return;

    qDeleteAll(m_items);
    m_items.clear();

    QVector<cuc::Item> transfereditems = m_transfer->collect();
    foreach (const cuc::Item &hubItem, transfereditems) {
        ContentItem *qmlItem = new ContentItem(this);
        qmlItem->setItem(hubItem);
        m_items.append(qmlItem);
    }
    Q_EMIT itemsChanged();
}

/*!
 * \brief ContentTransfer::updateState update the state from the hub transfer object
 */
void ContentTransfer::updateState()
{
    qDebug() << Q_FUNC_INFO;
    if (!m_transfer)
        return;

    m_state = static_cast<ContentTransfer::State>(m_transfer->state());
    Q_EMIT stateChanged();
}

/*!
 * \brief ContentTransfer::updateSelectionType update the selectionType from the hub transfer object
 */
void ContentTransfer::updateSelectionType()
{
    qDebug() << Q_FUNC_INFO;
    if (!m_transfer)
        return;

    m_selectionType = static_cast<ContentTransfer::SelectionType>(m_transfer->selectionType());
    Q_EMIT selectionTypeChanged();
}


/*!
 * \brief ContentTransfer::updateStore update the store from the hub transfer object
 */
void ContentTransfer::updateStore()
{
    qDebug() << Q_FUNC_INFO;
    if (!m_transfer)
        return;

    m_store = m_transfer->store();
    Q_EMIT storeChanged();
}
