/*
 * Copyright (C) 2013 Canonical, Ltd.
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

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QTimer>
#include <com/ubuntu/content/peer.h>

#include "hook.h"

Hook::Hook(QObject *parent) :
    QObject(parent),
    registry(new Registry())
{
    QTimer::singleShot(200, this, SLOT(run()));
}


Hook::Hook(Registry *registry, QObject *parent) :
    QObject(parent),
    registry(registry)
{
}

void Hook::run()
{
    qDebug() << Q_FUNC_INFO;
    QDir contentDir(
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
        + QString("/")
        + QString("content-hub"));

    if (not contentDir.exists())
        return_error();

    QStringList all_peers;
    registry->enumerate_known_peers([&all_peers](const com::ubuntu::content::Peer& peer)
                                    {
                                        all_peers.append(peer.id());
                                    });

    Q_FOREACH(QString p, all_peers)
    {
        qDebug() << Q_FUNC_INFO << "Looking for" << p;
        QStringList pp = contentDir.entryList(QStringList("*"+ p));
        if (pp.isEmpty())
            registry->remove_peer(com::ubuntu::content::Peer{p});
    }

    Q_FOREACH(QFileInfo f, contentDir.entryInfoList(QDir::Files))
        add_peer(f);

    QCoreApplication::instance()->quit();
}

bool Hook::add_peer(QFileInfo result)
{
    qDebug() << Q_FUNC_INFO << "Hook:" << result.filePath();

    QString app_id = result.fileName();
    auto peer = cuc::Peer(app_id);

    QFile contentJson(result.absoluteFilePath());
    if (!contentJson.open(QIODevice::ReadOnly | QIODevice::Text))
        return_error("couldn't open " + result.absoluteFilePath());

    QJsonParseError *e = new QJsonParseError();
    QJsonDocument contentDoc = QJsonDocument::fromJson(contentJson.readAll(), e);

    if (e->error != 0)
        return return_error(e->errorString());

    if (not contentDoc.isObject())
        return return_error("invalid JSON object");

    QJsonObject contentObj = contentDoc.object();
    QVariant sources = contentObj.toVariantMap()["source"];
    Q_FOREACH(QString source, sources.toStringList())
    {
        /* FIXME: we should iterate known types, but there isn't
         * really a good way to do that right now */
        if (source == "pictures")
        {
            if (not registry->install_peer_for_type(cuc::Type::Known::pictures(), peer))
                qWarning() << "Failed to install peer for" << source;
        }
        else if (source == "music")
        {
            if (not registry->install_peer_for_type(cuc::Type::Known::music(), peer))
                qWarning() << "Failed to install peer for" << source;
        }
        else if (source == "documents")
        {
            if (not registry->install_peer_for_type(cuc::Type::Known::documents(), peer))
                qWarning() << "Failed to install peer for" << source;
        }
    }
    return true;
}

bool Hook::return_error(QString err)
{
    qWarning() << "Failed to install peer" << err;
    return false;
}
