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

#ifndef COM_UBUNTU_CONTENTPEERMODEL_H_
#define COM_UBUNTU_CONTENTPEERMODEL_H_

#include "contentpeer.h"
#include "contenttype.h"
#include "contenthandler.h"
#include <com/ubuntu/content/hub.h>

#include <QObject>
#include <QVariant>
#include <QQmlListProperty>
#include <QQmlParserStatus>

class ContentPeerModel : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(ContentType::Type contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(QStringList mimeTypes READ mimeTypes WRITE setMimeTypes NOTIFY mimeTypesChanged)
    Q_PROPERTY(ContentHandler::Handler handler READ handler WRITE setHandler NOTIFY handlerChanged)
    Q_PROPERTY(QQmlListProperty<ContentPeer> peers READ peers NOTIFY peersChanged)

public:
    ContentPeerModel(QObject *parent = nullptr);

    void classBegin();
    void componentComplete();
    ContentType::Type contentType();
    void setContentType(ContentType::Type contentType);
    void appendPeersForContentType(ContentType::Type contentType);
    QStringList mimeTypes();
    void setMimeTypes(QStringList mimeTypes);
    ContentHandler::Handler handler();
    void setHandler(ContentHandler::Handler handler);
    QQmlListProperty<ContentPeer> peers();

Q_SIGNALS:
    void contentTypeChanged();
    void contentMimeTypesChanged();
    void handlerChanged();
    void peersChanged();
    void findPeersCompleted();

public Q_SLOTS:
    void findPeers();

private:
    com::ubuntu::content::Hub *m_hub;
    ContentType::Type m_contentType;
    QStringList m_mimeTypes;
    ContentHandler::Handler m_handler;
    QList<ContentPeer *> m_peers;
    bool m_complete;
};

#endif // COM_UBUNTU_CONTENTPEERMODEL_H_
