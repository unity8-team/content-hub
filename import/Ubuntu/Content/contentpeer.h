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

#ifndef COM_UBUNTU_CONTENTPEER_H_
#define COM_UBUNTU_CONTENTPEER_H_

#include <com/ubuntu/content/peer.h>

#include <QObject>
#include <QString>
#include <QImage>
class ContentPeer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(QImage icon READ icon)

public:
    ContentPeer(QObject *parent = nullptr);

    QString name();
    const QString &appId() const;
    void setAppId(const QString&);
    QImage &icon();

    const com::ubuntu::content::Peer &peer() const;
    void setPeer(const com::ubuntu::content::Peer &peer);

Q_SIGNALS:
    void nameChanged();
    void appIdChanged();

private:
    com::ubuntu::content::Peer m_peer;
    QImage m_icon;
};

#endif // COM_UBUNTU_CONTENTPEER_H_
