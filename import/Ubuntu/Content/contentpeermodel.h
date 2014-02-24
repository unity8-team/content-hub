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
#include <QVariantList>

class ContentPeerModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ContentType::Type contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(ContentHandler::Handler handler READ handler WRITE setHandler NOTIFY handlerChanged)
    Q_PROPERTY(QVariantList peers READ peers NOTIFY peersChanged)

public:
    ContentPeerModel(QObject *parent = nullptr);

    ContentType::Type contentType();
    void setContentType(ContentType::Type contentType);
    ContentHandler::Handler handler();
    void setHandler(ContentHandler::Handler handler);
    QVariantList peers();

Q_SIGNALS:
    void contentTypeChanged();
    void handlerChanged();
    void peersChanged();

private:
    com::ubuntu::content::Hub *m_hub;
    ContentType::Type m_contentType;
    ContentHandler::Handler m_handler;
    QVariantList m_peers;
};

#endif // COM_UBUNTU_CONTENTPEERMODEL_H_
