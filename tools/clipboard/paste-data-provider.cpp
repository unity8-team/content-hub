/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "paste-data-provider.h"

#include "debug.h"
#include "utils.cpp"

PasteDataProvider::PasteDataProvider(QObject* parent)
    : QObject(parent),
    d{new PasteDataProviderPrivate{this}}
{
}

PasteDataProvider::~PasteDataProvider()
{
}

QDBusPendingCall PasteDataProvider::requestAllPasteIds(const QString &surfaceId)
{
    TRACE() << Q_FUNC_INFO;
    return d->service->GetAllPasteIds(surfaceId);
}

QDBusPendingCall PasteDataProvider::requestPasteById(const QString &surfaceId, int pasteId)
{
    TRACE() << Q_FUNC_INFO;
    return d->service->GetPasteData(surfaceId, QString::number(pasteId));
}

QDBusPendingCall PasteDataProvider::requestRemovePaste(const QString &surfaceId, int pasteId)
{
    TRACE() << Q_FUNC_INFO;
    return d->service->RemovePaste(surfaceId, QString::number(pasteId));
}

QStringList PasteDataProvider::allPasteIds(const QString &surfaceId)
{
    QDBusPendingCall pendingCall = requestAllPasteIds(surfaceId);
    auto reply = QDBusPendingReply<QStringList>(pendingCall);
    reply.waitForFinished();

    if (reply.isError())
        return QStringList();

    return qdbus_cast<QStringList>(reply.value());
}

QMimeData* PasteDataProvider::pasteById(const QString &surfaceId, int pasteId)
{
    QDBusPendingCall pendingCall = requestPasteById(surfaceId, pasteId);
    auto reply = QDBusPendingReply<QByteArray>(pendingCall);
    reply.waitForFinished();

    if (reply.isError())
        return nullptr;

    QByteArray serializedMimeData = qdbus_cast<QByteArray>(reply.value());
    return deserializeMimeData(serializedMimeData);
}

bool PasteDataProvider::removePaste(const QString &surfaceId, int pasteId)
{
    QDBusPendingCall pendingCall = requestRemovePaste(surfaceId, pasteId);
    auto reply = QDBusPendingReply<bool>(pendingCall);
    reply.waitForFinished();

    if (reply.isError())
        return false;

    return qdbus_cast<bool>(reply.value());
}
