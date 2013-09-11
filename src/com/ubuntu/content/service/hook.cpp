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

#include "hook.h"

Hook::Hook(QString app_id, QObject *parent) :
    QObject(parent),
    app_id(app_id),
    registry(new Registry())
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(200, this, SLOT(run()));
}

void Hook::run()
{
    qDebug() << Q_FUNC_INFO;

    /* FIXME: we should do a sanity check on this before installing */
    auto peer = cuc::Peer(app_id);

    QDir contentDir(
        QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)
        + QString("/")
        + QString("content-hub"));

    qDebug() << contentDir.absolutePath();
    if (not contentDir.exists())
        return_error();

    const QString file = contentDir.filePath(peer.id() + QString("-content.json"));

    QFile contentJson(file);
    if (!contentJson.open(QIODevice::ReadOnly | QIODevice::Text))
        return_error("couldn't open " + QString(file));

    QJsonParseError *e = new QJsonParseError();
    QJsonDocument contentDoc = QJsonDocument::fromJson(contentJson.readAll(), e);

    if (e->error != 0)
        return_error(e->errorString());

    if (not contentDoc.isObject())
        return_error("invalid JSON object");


    QJsonObject contentObj = contentDoc.object();
    QVariant sources = contentObj.toVariantMap()["source"];
    Q_FOREACH(QString source, sources.toStringList())
    {
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

    QCoreApplication::instance()->quit();
}

void Hook::return_error(QString err)
{
    qWarning() << "Failed to install peer" << err;
    QCoreApplication::instance()->exit(1);
}
