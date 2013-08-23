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

#include <com/ubuntu/content/transfer.h>

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

ContentTransfer::ContentTransfer(QObject *parent)
    : QObject(parent),
      m_transfer(0)
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
    return Aborted;
}

/*!
 * \qmlproperty list<ContentItem> ContentTransfer::items
 *
 * FIXME add documentation
 */
QQmlListProperty<ContentItem> ContentTransfer::items()
{
    qDebug() << Q_FUNC_INFO;
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

    qDebug() << Q_FUNC_INFO;

    m_transfer = transfer;
    connect(m_transfer, SIGNAL(stateChanged()), this, SIGNAL(stateChanged()));
}
