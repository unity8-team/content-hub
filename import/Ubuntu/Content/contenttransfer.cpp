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
#include "../../../src/com/ubuntu/content/debug.h"

#include <com/ubuntu/content/item.h>

/*!
 * \qmltype ContentTransfer
 * \instantiates ContentTransfer
 * \inqmlmodule Ubuntu.Content
 * \brief Represents a transfer of content between two ContentPeers
 *
 * See documentation for ContentHub
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
    TRACE() << Q_FUNC_INFO;
}

/*!
   \qmlproperty ContentTransfer.State ContentTransfer::state

   \table
   \header
     \li {2, 1} \e {ContentTransfer.State} is an enumeration:
   \header
     \li State
     \li Description
   \row
     \li ContentTransfer.Created
     \li Transfer created, waiting to be initiated.
   \row
     \li ContentTransfer.Initiated
     \li Transfer has been initiated.
   \row
     \li ContentTransfer.InProgress
     \li Transfer is in progress.
   \row
     \li ContentTransfer.Charged
     \li Transfer is charged with items and ready to be collected.
   \row
     \li ContentTransfer.Collected
     \li Items in the transfer have been collected.
   \row
     \li ContentTransfer.Aborted
     \li Transfer has been aborted.
   \row
     \li ContentTransfer.Finalized
     \li Transfer has been finished and cleaned up.
   \endtable
 */
ContentTransfer::State ContentTransfer::state() const
{
    TRACE() << Q_FUNC_INFO;
    return m_state;
}

void ContentTransfer::setState(ContentTransfer::State state)
{
    TRACE() << Q_FUNC_INFO << state;
    if (!m_transfer)
        return;

    if (state == Charged && m_state == InProgress) {
        TRACE() << Q_FUNC_INFO << "Charged";
        QVector<cuc::Item> hubItems;
        hubItems.reserve(m_items.size());
        Q_FOREACH (const ContentItem *citem, m_items) {
            hubItems.append(citem->item());
        }
        m_transfer->charge(hubItems);
        return;
    } else if (state == Aborted) {
        TRACE() << Q_FUNC_INFO << "Aborted";
        m_transfer->abort();
    } else
        updateState();
}

/*!
  \qmlproperty ContentTransfer.Direction ContentTransfer::direction
  \brief ContentTransfer::direction indicates if this transferobject is used for
  import or export transaction
  \table
  \header
     \li {2, 1} \e {ContentTransfer.Direction} is an enumeration:
  \header
    \li Direction
    \li Description
  \row
    \li ContentTransfer.Import
    \li Transfer is a request to import content.
  \row
    \li ContentTransfer.Export
    \li Transfer is a request to export content.
  \row
    \li ContentTransfer.Share
    \li Transfer is a request to share content.
  \endtable
 */
ContentTransfer::Direction ContentTransfer::direction() const
{
    return m_direction;
}

/*!
  \qmlproperty ContentTransfer.SelectionType ContentTransfer::selectionType
  \brief ContentTransfer::selectionType indicates if this transfer object allows
  single or multiple selection of items
  \table
  \header
     \li {2, 1} \e {ContentTransfer.SelectionType} is an enumeration:
  \header
    \li Type
    \li Description
  \row
    \li ContentTransfer.Single
    \li Transfer should contain a single item.
  \row
    \li ContentTransfer.Multiple
    \li Transfer can contain multiple items.
  \endtable
 */
ContentTransfer::SelectionType ContentTransfer::selectionType() const
{
    return m_selectionType;
}

void ContentTransfer::setSelectionType(ContentTransfer::SelectionType type)
{
    TRACE() << Q_FUNC_INFO << type;
    if (!m_transfer)
        return;

    if (m_state == Created && (m_selectionType != type)) {
        m_transfer->setSelectionType(static_cast<cuc::Transfer::SelectionType>(type));
    }
}

/*!
 * \qmlproperty list<ContentItem> ContentTransfer::items
 *
 * List of items included in the ContentTransfer
 */
QQmlListProperty<ContentItem> ContentTransfer::items()
{
    TRACE() << Q_FUNC_INFO;
    if (m_state == Charged) {
        collectItems();
    }
    return QQmlListProperty<ContentItem>(this, m_items);
}

/*!
 * \qmlmethod ContentTransfer::start()
 *
 *  Start the transfer, this sets the State to ContentTransfer.Initiated
 */
bool ContentTransfer::start()
{
    TRACE() << Q_FUNC_INFO << m_transfer->id() << ":" << m_state;
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
 * Sets State to ContentTransfer.Finalized and cleans up temporary files
 */
bool ContentTransfer::finalize()
{
    TRACE() << Q_FUNC_INFO;
    return m_transfer->finalize();
}

/*!
 * \qmlproperty string ContentTransfer::store
 * ContentStore where the ContentTransfer will add items
 */
const QString ContentTransfer::store() const
{
    TRACE() << Q_FUNC_INFO;
    return m_transfer->store().uri();
}

void ContentTransfer::setStore(ContentStore* contentStore)
{
    TRACE() << Q_FUNC_INFO;

    if (!m_transfer)
    {
        qWarning() << Q_FUNC_INFO << "invalid transfer";
        return;
    }

    if(contentStore->store() != nullptr) {
        m_transfer->setStore(contentStore->store());
    }
}

/*!
 * \brief ContentTransfer::transfer
 * \internal
 */
com::ubuntu::content::Transfer *ContentTransfer::transfer() const
{
    TRACE() << Q_FUNC_INFO;
    return m_transfer;
}

/*!
 * \brief ContentTransfer::setTransfer
 * \internal
 */
void ContentTransfer::setTransfer(com::ubuntu::content::Transfer *transfer)
{
    if (m_transfer) {
        qWarning() << Q_FUNC_INFO << "the transfer object was already set";
        return;
    }

    if (!transfer) {
        qWarning() << Q_FUNC_INFO << "No valid transfer object passed:" << transfer;
        return;
    }

    m_transfer = transfer;
    m_direction = static_cast<ContentTransfer::Direction>(transfer->direction());
    TRACE() << Q_FUNC_INFO << "Direction:" << m_direction;

    connect(m_transfer, SIGNAL(selectionTypeChanged()), this, SLOT(updateSelectionType()));
    connect(m_transfer, SIGNAL(storeChanged()), this, SLOT(updateStore()));
    connect(m_transfer, SIGNAL(stateChanged()), this, SLOT(updateState()));

    updateSelectionType();
    updateStore();
    updateState();
}

/*!
 * \brief ContentTransfer::collectItems gets the items out of the transfer object
 * \internal
 */
void ContentTransfer::collectItems()
{
    TRACE() << Q_FUNC_INFO;
    if (m_state != Charged)
        return;

    qDeleteAll(m_items);
    m_items.clear();

    QVector<cuc::Item> transfereditems = m_transfer->collect();
    Q_FOREACH (const cuc::Item &hubItem, transfereditems) {
        ContentItem *qmlItem = new ContentItem(this);
        qmlItem->setItem(hubItem);
        m_items.append(qmlItem);
    }
    Q_EMIT itemsChanged();
}

/*!
 * \brief ContentTransfer::updateState update the state from the hub transfer object
 * \internal
 */
void ContentTransfer::updateState()
{
    TRACE() << Q_FUNC_INFO << m_transfer->state();

    if (!m_transfer)
    {
        TRACE() << Q_FUNC_INFO << "Invalid transfer";
        return;
    }

    m_state = static_cast<ContentTransfer::State>(m_transfer->state());
    Q_EMIT stateChanged();
}

/*!
 * \brief ContentTransfer::updateSelectionType update the selectionType from the hub transfer object
 * \internal
 */
void ContentTransfer::updateSelectionType()
{
    TRACE() << Q_FUNC_INFO;
    if (!m_transfer)
    {
        TRACE() << Q_FUNC_INFO << "Invalid transfer";
        return;
    }

    m_selectionType = static_cast<ContentTransfer::SelectionType>(m_transfer->selectionType());
    Q_EMIT selectionTypeChanged();
}


/*!
 * \brief ContentTransfer::updateStore update the store from the hub transfer object
 * \internal
 */
void ContentTransfer::updateStore()
{
    TRACE() << Q_FUNC_INFO;
    if (!m_transfer)
    {
        TRACE() << Q_FUNC_INFO << "Invalid transfer";
        return;
    }

    m_store = m_transfer->store();
    Q_EMIT storeChanged();
}
