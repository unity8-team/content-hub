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
#include <QDir>
#include <QFile>
#include <QFileInfo>

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
    return m_item.name();
}

void ContentItem::setName(const QString &name)
{
    TRACE() << Q_FUNC_INFO;
    if (name == m_item.name())
        return;

    m_item.setName(name);
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

    QString oldName = m_item.name();
    m_item = cuc::Item(url);
    m_item.setName(oldName);
    Q_EMIT urlChanged();
}

/*!
 * \qmlproperty string ContentItem::text
 * Content of the transfer
 */
QString ContentItem::text()
{
    TRACE() << Q_FUNC_INFO;
    return QString(m_item.text());
}

void ContentItem::setText(const QString &text)
{
    TRACE() << Q_FUNC_INFO;
    if (text == QString(m_item.text()))
        return;

    m_item.setText(text);
    Q_EMIT textChanged();
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

/*!
 * \qmlmethod string ContentItem::toDataURI
 *  Returns the ContentItem base64 encoded with the mimetype as a 
 *  properly formated dataUri
 */
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

/*!
 * \qmlmethod bool ContentItem::move(dir)
 * \brief If the url is a local file, move the file to \a dir
 *
 *  If the move is successful, the url property will be changed
 *  and onUrlChanged will be emitted.
 *
 *  Returns true if the file was moved successfully, false 
 *  on error or if the url wasn't a local file.
 */
bool ContentItem::move(const QString &dir)
{
    TRACE() << Q_FUNC_INFO << "dir:" << dir;
    return (move(dir, nullptr));
}

/*!
 * \qmlmethod bool ContentItem::move(dir, fileName)
 * \brief If the url is a local file, move the file to \a dir and 
 *  rename to \a fileName
 *
 *  If the move is successful, the url property will be changed
 *  and onUrlChanged will be emitted.
 *
 *  Returns true if the file was moved successfully, false 
 *  on error or if the url wasn't a local file.
 */
bool ContentItem::move(const QString &dir, const QString &fileName)
{
    TRACE() << Q_FUNC_INFO << "dir:" << dir << "fileName:" << fileName;

    QString path(m_item.url().toLocalFile());

    if (!QFile::exists(path)) {
        qWarning() << "File not found:" << path;
        return false;
    }

    QFileInfo fi(path);
    QDir d(dir);
    if (not d.exists())
        d.mkpath(d.absolutePath());

    QString destFilePath = "";
    if (fileName.isEmpty())
        destFilePath = dir + QDir::separator() + fi.fileName();
    else
        destFilePath = dir + QDir::separator() + fileName;

    TRACE() << Q_FUNC_INFO << "New path:" << destFilePath;

    if (not QFile::rename(fi.absoluteFilePath(), destFilePath)) {
        qWarning() << "Failed to move content to:" << destFilePath << "falling back to copy";
        if (not QFile::copy(fi.absoluteFilePath(), destFilePath)) {
            qWarning() << "Failed to copy content to:" << destFilePath;
            return false;
        }
    }

    setUrl(QUrl::fromLocalFile(destFilePath));
    return true;
}
