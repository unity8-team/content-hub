/*
 * Copyright (C) 2016 Canonical, Ltd.
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
 * Authored by: Arthur Mello <arthur.mello@canonical.com>
 */

#ifndef PASTEDATAPROVIDER_H
#define PASTEDATAPROVIDER_H

#include <QObject>

#include "ContentServiceInterface.h"

const QLatin1String SERVICE_NAME = QLatin1String("com.ubuntu.content.dbus.Service");
const QLatin1String SERVICE_PATH = QLatin1String("/");

struct PasteDataProviderPrivate
{
    PasteDataProviderPrivate(QObject* parent) : service(
            new com::ubuntu::content::dbus::Service(
                SERVICE_NAME,
                SERVICE_PATH,
                QDBusConnection::sessionBus(),
                parent))
    {
    }

    com::ubuntu::content::dbus::Service* service;
};

class PasteDataProvider : public QObject
{
    Q_OBJECT

public:
    PasteDataProvider(QObject* parent=0);
    ~PasteDataProvider();

    struct PasteData
    {
        QString pasteId;
        QString source;
        QString pasteData;
    };

    QStringList allPasteIds(const QString &surfaceId);
    QMimeData* pasteDataById(const QString &surfaceId, int pasteId);
    QString pasteSourceById(const QString &surfaceId, int pasteId);
    bool removePaste(const QString &surfaceId, int pasteId);

Q_SIGNALS:
    void PasteboardChanged();

private:
    QDBusPendingCall requestAllPasteIds(const QString &surfaceId);
    QDBusPendingCall requestPasteDataById(const QString &surfaceId, int pasteId);
    QDBusPendingCall requestPasteSourceById(const QString &surfaceId, int pasteId);
    QDBusPendingCall requestRemovePaste(const QString &surfaceId, int pasteId);

    PasteDataProviderPrivate* d;
};

#endif // PASTEDATAPROVIDER_H
