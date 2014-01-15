/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include "example.h"
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/store.h>
#include <QCoreApplication>

namespace cuc = com::ubuntu::content;

Example::Example(QObject *parent) :
    QObject(parent)
{
}

void Example::create_share()
{
    auto hub = cuc::Hub::Client::instance();

    auto peer = cuc::Peer{"example-share-handler"};

    m_transfer = hub->create_share_for_peer(peer);

    /* listen for state changes so we known when it's collected */
    connect(m_transfer, SIGNAL(stateChanged()), this, SLOT(state_check()));

    m_transfer->start();
    QVector<cuc::Item> items;
    items << cuc::Item(QUrl("http:///example.com/test1.jpg"));
    items << cuc::Item(QUrl("file:///tmp/test2.jpg"));
    m_transfer->charge(items);
    qDebug() << Q_FUNC_INFO << "Items:" << items.count();
}

void Example::state_check()
{
    qDebug() << Q_FUNC_INFO << m_transfer->state();
    /* When the transfer is collected we'll know the share was successful */
    if (m_transfer->state() == cuc::Transfer::collected)
    {
        qDebug() << Q_FUNC_INFO << "Collected";
        QCoreApplication::exit();
    }

    /* If the transfer is aborted, we had a failure */
    if (m_transfer->state() == cuc::Transfer::aborted)
    {
        qDebug() << Q_FUNC_INFO << "Share failed";
        QCoreApplication::exit();
    }
}
