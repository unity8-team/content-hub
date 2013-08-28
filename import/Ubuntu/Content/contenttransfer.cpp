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
#include <contentitem.h>

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
      m_direction(Import)
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
    if (!m_transfer) {
        qWarning() << Q_FUNC_INFO << "no valid transfer object available";
        return false;
    }

    qDebug() << Q_FUNC_INFO;
    return m_transfer->start();
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
    updateSate();

    if (m_state == Charged && m_direction == Import)
        collectItems();

    connect(m_transfer, SIGNAL(stateChanged()), this, SLOT(updateSate()));
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
 * \brief ContentTransfer::updateSate update the state from the hub transfer object
 */
void ContentTransfer::updateSate()
{
    if (!m_transfer)
        return;

    m_state = static_cast<ContentTransfer::State>(m_transfer->state());
    Q_EMIT stateChanged();
}
