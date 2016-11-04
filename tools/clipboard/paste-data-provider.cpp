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

PasteDataProvider::PasteDataProvider(QObject* parent)
    : QObject(parent),
    d{new PasteDataProviderPrivate{this}}
{
}

PasteDataProvider::~PasteDataProvider()
{
}

QDBusPendingCall PasteDataProvider::requestAllPasteData()
{
    TRACE() << Q_FUNC_INFO;
    return d->service->GetAllPasteData();
}

QStringList PasteDataProvider::allPasteData()
{
    QDBusPendingCall pendingCall = requestAllPasteData();
    auto reply = QDBusPendingReply<QStringList>(pendingCall);
    reply.waitForFinished();

    if (reply.isError())
        return QStringList();

    return qdbus_cast<QStringList>(reply.value());
}
