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

#include "contentitem.h"
#include "../../../src/com/ubuntu/content/debug.h"
#include <QMimeDatabase>
#include <QFile>

/*!
 * \qmltype ContentItem
 * \instantiates ContentItem
 * \inqmlmodule Ubuntu.Content
 * \brief Content that can be imported or exported from a ContentPeer
 *
 * A ContentItem is an item that can be imported or exported from a ContentPeer
 *
 * See documentation for ContentHub
 */

namespace cuc = com::ubuntu::content;

ContentItem::ContentItem(QObject *parent)
    : QObject(parent)
{
    TRACE() << Q_FUNC_INFO;
}

/*!
 * \qmlproperty string ContentItem::name
 * \internal
 */
const QString &ContentItem::name() const
{
    TRACE() << Q_FUNC_INFO;
    return m_name;
}

void ContentItem::setName(const QString &name)
{
    TRACE() << Q_FUNC_INFO;
    if (name == m_name)
        return;

    m_name = name;
    Q_EMIT nameChanged();
}

/*!
 * \qmlproperty url ContentItem::url
 *
 * URL of the content data
 */
const QUrl &ContentItem::url() const
{
    TRACE() << Q_FUNC_INFO;
    return m_item.url();
}

void ContentItem::setUrl(const QUrl &url)
{
    TRACE() << Q_FUNC_INFO;
    if (url == this->url())
        return;

    m_item = cuc::Item(url);
    Q_EMIT urlChanged();
}

/*!
 * \brief ContentItem::item
 * \internal
 */
const com::ubuntu::content::Item &ContentItem::item() const
{
    TRACE() << Q_FUNC_INFO;
    return m_item;
}

/*!
 * \brief ContentItem::setItem
 * \internal
 */
void ContentItem::setItem(const com::ubuntu::content::Item &item)
{
    TRACE() << Q_FUNC_INFO;
    if (item == m_item)
        return;

    m_item = item;
    Q_EMIT urlChanged();
}

QUrl ContentItem::toDataURI()
{
    TRACE() << Q_FUNC_INFO;

    QString path(m_item.url().toLocalFile());

    /* Don't attempt to create the dataUri if the file isn't local */
    if (!QFile::exists(path)) {
        qWarning() << "File not found:" << path;
        return QUrl();
    }
    QMimeDatabase mdb;
    QMimeType mt = mdb.mimeTypeForFile(path);
    /* Don't attempt to create the dataUri if we can't detect the mimetype */
    if (!mt.isValid()) {
        qWarning() << "Unknown MimeType for file:" << path;
        return QUrl();
    }
    
    QString contentType(mt.name());
    QByteArray data;

    QFile file(path);
    if(file.open(QIODevice::ReadOnly)) {
        data = file.readAll();
        file.close();
    }

    /* Don't attempt to create the dataUri with empty data */
    if (data.isEmpty()) {
        qWarning() << "Failed to read contents of file:" << path;
        return QUrl();
    }

    QString dataUri(QStringLiteral("data:"));
    dataUri.append(contentType);
    dataUri.append(QStringLiteral(";base64,"));
    dataUri.append(QString::fromLatin1(data.toBase64()));
    return QUrl(dataUri);
}
