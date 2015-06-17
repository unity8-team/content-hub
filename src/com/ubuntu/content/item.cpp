/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#include <QDBusArgument>

#include <com/ubuntu/content/item.h>
#include "debug.h"

namespace cuc = com::ubuntu::content;

struct cuc::Item::Private
{
    QUrl url;
    QString name;
    QByteArray stream;
    QString streamType;

    bool operator==(const Private& rhs) const
    {
        return url == rhs.url && name == rhs.name && stream == rhs.stream && streamType == rhs.streamType;
    }
};

cuc::Item::Item(const QUrl& url, QObject* parent) : QObject(parent), d{new cuc::Item::Private{url, QString(), QByteArray(), QString()}}
{
}

cuc::Item::Item(const cuc::Item& rhs) : QObject(rhs.parent()), d(rhs.d)
{
}

cuc::Item& cuc::Item::operator=(const cuc::Item& rhs)
{
    d = rhs.d;
    return *this;
}

bool cuc::Item::operator==(const cuc::Item& rhs) const
{
    if (d == rhs.d)
        return true;

    return *d == *rhs.d;
}

cuc::Item::~Item()
{
}

const QUrl& cuc::Item::url() const
{
    return d->url;
}

void cuc::Item::setUrl(const QUrl& newUrl) const
{
    if (newUrl == d->url)
        return;

    d->url = newUrl;
}

const QString& cuc::Item::name() const
{
    return d->name;
}

void cuc::Item::setName(const QString& newName) const
{
    if (newName != d->name)
        d->name = newName;
}

const QString cuc::Item::text() const
{
    if (d->streamType == "plain/text")
        return QString(d->stream);
    return QString();
}

void cuc::Item::setText(const QString& text) const
{
    if (text == QString(d->stream))
        return;

    setStream(QByteArray::fromStdString(text.toStdString()));
    d->streamType = QString("plain/text");
}

const QByteArray& cuc::Item::stream() const
{
    return d->stream;
}

void cuc::Item::setStream(const QByteArray& newStream) const
{
    if (newStream != d->stream)
        d->stream = newStream;
}

const QString& cuc::Item::streamType() const
{   
    return d->streamType;
}

void cuc::Item::setStreamType(const QString& newStreamType) const
{
    if (newStreamType != d->streamType)
        d->streamType = newStreamType;
}

QDBusArgument &operator<<(QDBusArgument &argument, const cuc::Item& item)
{
    argument.beginStructure();
    argument << item.streamType() << item.stream() << item.name() << item.url().toDisplayString();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, cuc::Item &item)
{
    TRACE() << Q_FUNC_INFO;
    QString name;
    QString urlString;
    QByteArray stream;
    QString streamType;

    argument.beginStructure();
    argument >> streamType >> stream >> name >> urlString;
    argument.endStructure();

    item = cuc::Item{QUrl(urlString)};
    item.setName(name);
    item.setStream(stream);
    item.setStreamType(streamType);
    return argument;
}
