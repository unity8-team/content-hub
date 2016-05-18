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
#include "debug.h"

AutoExporter::AutoExporter()
{
    auto hub = cuc::Hub::Client::instance();
    hub->register_import_export_handler(this);
}

void AutoExporter::setUrl(const QString& newUrl)
{
    url = newUrl;
}

void AutoExporter::setText(const QString& newText)
{
    text = newText;
}

void AutoExporter::handle_import(cuc::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoExporter::handle_copy(cuc::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoExporter::handle_paste(cuc::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoExporter::handle_export(cuc::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO;
    if (transfer == nullptr) {
        TRACE() << Q_FUNC_INFO << "Transfer null";
        return;
    }

    cuc::Item item;

    if (!url.isEmpty())
        item.setUrl(QUrl(url));
    if (!text.isEmpty())
        item.setText(text);

    QVector<cuc::Item> items;
    items << item;
    transfer->charge(items);
    connect(transfer, SIGNAL(stateChanged()), this, SLOT(stateChanged()));
    TRACE() << Q_FUNC_INFO << "Items:" << items.count();
}

void AutoExporter::handle_share(cuc::Transfer *transfer)
{
    TRACE() << Q_FUNC_INFO << "not implemented";
    Q_UNUSED(transfer);
}

void AutoExporter::stateChanged()
{
    cuc::Transfer *transfer = static_cast<cuc::Transfer*>(sender());
    TRACE() << Q_FUNC_INFO << "STATE:" << transfer->state();

    if (transfer->state() == cuc::Transfer::aborted)
        QCoreApplication::instance()->exit(1);
    if (transfer->state() == cuc::Transfer::collected)
        QCoreApplication::instance()->exit(0);
}
