/*
 * Copyright (C) 2014 Canonical, Ltd.
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

#include "autosharer.h"

AutoSharer::AutoSharer()
{
    auto hub = cuc::Hub::Client::instance();
    hub->register_import_export_handler(this);
}

void AutoSharer::handle_import(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoSharer::handle_export(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoSharer::handle_share(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO;
    if (transfer == nullptr)
    {
        qDebug() << Q_FUNC_INFO << "Transfer null";
        return;
    }

    connect(transfer, SIGNAL(stateChanged()), this, SLOT(stateChanged()));

    QVector<cuc::Item> items;
    items = transfer->collect();
    qDebug() << Q_FUNC_INFO << "Items:" << items.count();
}

void AutoSharer::stateChanged()
{
    qDebug() << Q_FUNC_INFO;
    cuc::Transfer *transfer = static_cast<cuc::Transfer*>(sender());

    qDebug() << Q_FUNC_INFO << "STATE:" << transfer->state();

    if (transfer->state() == cuc::Transfer::collected)
        QCoreApplication::instance()->quit();
}


