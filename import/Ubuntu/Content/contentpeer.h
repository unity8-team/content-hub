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

#include "contenthandler.h"
#include "contenttransfer.h"
#include "contenttype.h"
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/peer.h>

#include <QObject>
#include <QString>
#include <QImage>

class ContentPeer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
    Q_PROPERTY(ContentHandler::Handler handler READ handler WRITE setHandler NOTIFY handlerChanged)
    Q_PROPERTY(ContentType::Type contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(ContentTransfer::SelectionType selectionType READ selectionType WRITE setSelectionType NOTIFY selectionTypeChanged)
    Q_PROPERTY(QImage icon READ icon)
    Q_PROPERTY(bool defaultPeer READ defaultPeer)

public:
    ContentPeer(QObject *parent = nullptr);
    ContentPeer(ContentType::Type type, QObject *parent);

    Q_INVOKABLE ContentTransfer* request();
    Q_INVOKABLE ContentTransfer* request(ContentStore *store);

    QString name();
    const QString &appId() const;
    void setAppId(const QString&);
    QImage &icon();

    const com::ubuntu::content::Peer &peer() const;
    void setPeer(const com::ubuntu::content::Peer &peer);

    ContentHandler::Handler handler();
    void setHandler(ContentHandler::Handler handler);

    ContentType::Type contentType();
    void setContentType(ContentType::Type contentType);

    ContentTransfer::SelectionType selectionType();
    void setSelectionType(ContentTransfer::SelectionType selectionType);

    bool defaultPeer();

Q_SIGNALS:
    void nameChanged();
    void appIdChanged();
    void handlerChanged();
    void contentTypeChanged();
    void selectionTypeChanged();

private:
    void init();

    com::ubuntu::content::Hub *m_hub;
    com::ubuntu::content::Peer m_peer;
    ContentHandler::Handler m_handler;
    ContentType::Type m_contentType;
    ContentTransfer::SelectionType m_selectionType;
    bool m_explicit_app;
    QImage m_icon;
};

#endif // COM_UBUNTU_CONTENTPEER_H_
