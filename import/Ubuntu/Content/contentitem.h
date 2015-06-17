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

#ifndef COM_UBUNTU_CONTENTITEM_H_
#define COM_UBUNTU_CONTENTITEM_H_

#include <com/ubuntu/content/item.h>

#include <QObject>
#include <QString>
#include <QUrl>

class ContentItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    ContentItem(QObject *parent = nullptr);

    const QString &name() const;
    void setName(const QString &name);

    const QUrl &url() const;
    void setUrl(const QUrl &url);

    QString text();
    void setText(const QString &text);

    const com::ubuntu::content::Item &item() const;
    void setItem(const com::ubuntu::content::Item &item);

    Q_INVOKABLE QUrl toDataURI();
    Q_INVOKABLE bool move(const QString &dir);
    Q_INVOKABLE bool move(const QString &dir, const QString &fileName);

Q_SIGNALS:
    void nameChanged();
    void urlChanged();
    void textChanged();

private:
    com::ubuntu::content::Item m_item;
};

#endif // COM_UBUNTU_CONTENTITEM_H_
