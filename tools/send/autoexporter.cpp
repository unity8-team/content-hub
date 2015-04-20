/*
 * Copyright (C) 2015 Canonical, Ltd.
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

#include "autoexporter.h"
#include <QDebug>

AutoExporter::AutoExporter()
{
    auto hub = cuc::Hub::Client::instance();
    hub->register_import_export_handler(this);
}

void AutoExporter::setUrl(const QString& newUrl)
{
    url = newUrl;
}

void AutoExporter::handle_import(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoExporter::handle_export(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO;
    if (transfer == nullptr) {
        qDebug() << Q_FUNC_INFO << "Transfer null";
        return;
    }

    QVector<cuc::Item> items;

    items << cuc::Item(QUrl(url));

    transfer->charge(items);

    connect(transfer, SIGNAL(stateChanged()), this, SLOT(stateChanged()));

    qDebug() << Q_FUNC_INFO << "Items:" << items.count();
}

void AutoExporter::handle_share(cuc::Transfer *transfer)
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoExporter::stateChanged()
{
    qDebug() << Q_FUNC_INFO;
    cuc::Transfer *transfer = static_cast<cuc::Transfer*>(sender());

    qDebug() << Q_FUNC_INFO << "STATE:" << transfer->state();


    if (transfer->state() == cuc::Transfer::collected)
        QCoreApplication::instance()->quit();
}


