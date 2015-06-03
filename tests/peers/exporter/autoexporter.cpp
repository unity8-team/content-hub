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

#include "autoexporter.h"

AutoExporter::AutoExporter()
{
    auto hub = cuc::Hub::Client::instance();
    hub->register_import_export_handler(this);
}

void AutoExporter::setUrl(QString url)
{
    qDebug() << Q_FUNC_INFO << url;
    m_url = url;
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

    qDebug() << Q_FUNC_INFO << transfer->contentType();

    QVector<cuc::Item> items;

    if (m_url.isEmpty()) {
        if (transfer->contentType() == cuc::Type::Known::contacts().id()) {
            items << cuc::Item(QUrl("file:///usr/share/content-hub/testability/data/Joker.vcf"));
            if (transfer->selectionType() == cuc::Transfer::SelectionType::multiple) {
                items << cuc::Item(QUrl("file:///usr/share/content-hub/testability/data/Stark,_Tony.vcf"));
            }
        } else {
            items << cuc::Item(QUrl("file:///usr/share/content-hub/testability/data/webbrowser-app.png"));
            if (transfer->selectionType() == cuc::Transfer::SelectionType::multiple) {
                items << cuc::Item(QUrl("file:///usr/share/content-hub/testability/data/clock.png"));
            }
        }
    } else {
        items << cuc::Item(QUrl(m_url));
    }

    transfer->charge(items);

    connect(transfer, SIGNAL(stateChanged()), this, SLOT(stateChanged()));

    qDebug() << Q_FUNC_INFO << "Items:" << items.count();
    Q_FOREACH(cuc::Item item, items) {
        qDebug() << Q_FUNC_INFO << "URL:" << item.url();
        qDebug() << Q_FUNC_INFO << "Name:" << item.name();
        qDebug() << Q_FUNC_INFO << "Text:" << item.text();
        qDebug() << Q_FUNC_INFO << "StreamType:" << item.streamType();
    }
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

    if (transfer->state() == cuc::Transfer::aborted)
        QCoreApplication::instance()->exit(1);

    if (transfer->state() == cuc::Transfer::collected)
        QCoreApplication::instance()->exit(0);
}


