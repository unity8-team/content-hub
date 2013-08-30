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

#include "common.h"
#include "ContentServiceInterface.h"
#include "ContentHandlerInterface.h"
#include "handleradaptor.h"
#include "transfer_p.h"

#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/import_export_handler.h>
#include <com/ubuntu/content/peer.h>
#include <com/ubuntu/content/scope.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/type.h>
#include "utils.cpp"

#include <QStandardPaths>

#include <map>

namespace cuc = com::ubuntu::content;

struct cuc::Hub::Private
{
    Private(QObject* parent) : service(
        new com::ubuntu::content::dbus::Service(
            HUB_SERVICE_NAME,
            HUB_SERVICE_PATH,
            QDBusConnection::sessionBus(),
            parent))
    {
    }

    com::ubuntu::content::dbus::Service* service;
};

cuc::Hub::Hub(QObject* parent) : QObject(parent), d{new cuc::Hub::Private{this}}
{
}

cuc::Hub::~Hub()
{
}

cuc::Hub* cuc::Hub::Client::instance()
{
    static cuc::Hub* hub = new cuc::Hub(nullptr);
    return hub;
}

void cuc::Hub::register_import_export_handler(cuc::ImportExportHandler* handler)
{
    qDebug() << Q_FUNC_INFO;
    QString id = app_id();
    if (id.isEmpty())
    {
        qWarning() << "APP_ID isn't set, the handler can not be registered";
        return;
    }

    QString bus_name = handler_address(id);
    qDebug() << Q_FUNC_INFO << "BUS_NAME:" << bus_name;

    auto c = QDBusConnection::sessionBus();
    auto h = new cuc::detail::Handler(c, id, handler);

    new HandlerAdaptor(h);
    if (not c.registerService(bus_name))
    {
        qWarning() << Q_FUNC_INFO << "Failed to register name:" << bus_name;
        return;
    }

    if (not c.registerObject(HANDLER_PATH, h))
    {
        qWarning() << Q_FUNC_INFO << "Failed to register object for:" << bus_name;
        return;
    }

    d->service->RegisterImportExportHandler(
                QString(""),
                id,
                QDBusObjectPath{HANDLER_PATH});
}

const cuc::Store* cuc::Hub::store_for_scope_and_type(cuc::Scope scope, cuc::Type type)
{
    static const std::map<std::pair<cuc::Scope, cuc::Type>, cuc::Store*> lut =
            {
                {{cuc::system, cuc::Type::Known::pictures()}, new cuc::Store{"/content/Pictures", this}},
                {{cuc::system, cuc::Type::Known::music()}, new cuc::Store{"/content/Music", this}},
                {{cuc::system, cuc::Type::Known::documents()}, new cuc::Store{"/content/Documents", this}},
                {{cuc::user, cuc::Type::Known::pictures()}, new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), this}},
                {{cuc::user, cuc::Type::Known::music()}, new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::MusicLocation), this}},
                {{cuc::user, cuc::Type::Known::documents()}, new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), this}},
                {{cuc::app, cuc::Type::Known::pictures()}, new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Pictures", this}},
                {{cuc::app, cuc::Type::Known::music()}, new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Music", this}},
                {{cuc::app, cuc::Type::Known::documents()}, new cuc::Store{QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/Documents", this}},
            };

    auto it = lut.find(std::make_pair(scope, type));

    if (it == lut.end())
        return nullptr;

    return it->second;
}

cuc::Peer cuc::Hub::default_peer_for_type(cuc::Type t)
{
    auto reply = d->service->DefaultPeerForType(t.id());
    reply.waitForFinished();

    if (reply.isError())
        return cuc::Peer::unknown();

    return cuc::Peer(reply.value(), this);
}

QVector<cuc::Peer> cuc::Hub::known_peers_for_type(cuc::Type t)
{
    QVector<cuc::Peer> result;

    auto reply = d->service->KnownPeersForType(t.id());
    reply.waitForFinished();

    if (reply.isError())
        return result;
    
    auto ids = reply.value();

    Q_FOREACH(const QString& id, ids)
    {
        result << cuc::Peer(id, this);
    }

    return result;
}

cuc::Transfer* cuc::Hub::create_import_for_type_from_peer(cuc::Type type, cuc::Peer peer)
{
    /* This needs to be replaced with a better way to get the APP_ID */
    QString id = app_id();
    if (id == "")
        id = "NoAppId";

    auto reply = d->service->CreateImportForTypeFromPeer(type.id(), peer.id(), id);
    reply.waitForFinished();

    if (reply.isError())
        return nullptr;

    return cuc::Transfer::Private::make_transfer(reply.value(), this);
}

void cuc::Hub::quit()
{
    d->service->Quit();
}
