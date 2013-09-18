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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include <QtCore>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <nih/alloc.h>
#include <nih-dbus/dbus_util.h>

#include "common.h"

namespace {

/* sanitize the dbus names */
QString sanitize_id(const QString& appId)
{
    qDebug() << Q_FUNC_INFO;
    return QString(nih_dbus_path(NULL,
                                 "",
                                 appId.toLocal8Bit().data(),
                                 NULL)).remove(0, 1);
}

/* define a bus_name based on our namespace and the app_id */
QString handler_address(QString app_id)
{
    return QString(HANDLER_NAME_TEMPLATE).arg(sanitize_id(app_id));
}

QString handler_path(QString app_id)
{
    return nih_dbus_path(NULL,
                         HANDLER_BASE_PATH.data(),
                         app_id.toLocal8Bit().data(),
                         NULL);
}


QString app_id()
{
    /* FIXME: rely on APP_ID from env for now,
     * but we'll use this function as a place to
     * later use the application manager
     */
    return QString(qgetenv("APP_ID"));
}


QString aa_profile(QString uniqueConnectionId)
{
    qDebug() << Q_FUNC_INFO << uniqueConnectionId;
    QDBusMessage msg =
        QDBusMessage::createMethodCall("org.freedesktop.DBus",
                                       "/org/freedesktop/DBus",
                                       "org.freedesktop.DBus",
                                       "GetConnectionAppArmorSecurityContext");
    QString aaProfile;
    QVariantList args;
    args << uniqueConnectionId;
    msg.setArguments(args);
    QDBusMessage reply =
        QDBusConnection::sessionBus().call(msg, QDBus::Block);
    if (reply.type() == QDBusMessage::ReplyMessage) {
        aaProfile = reply.arguments().value(0, QString()).toString();
        qDebug() << "AppArmor Profile:" << aaProfile;
    } else {
        qWarning() << "Error getting app ID:" << reply.errorName() <<
            reply.errorMessage();
    }

    if (aaProfile.toStdString() == QString("unconfined").toStdString())
    {
        return QString("");
    }
    return aaProfile;
}

QString copy_to_store(const QString& src, const QString& store)
{
    qDebug() << Q_FUNC_INFO;
    QUrl srcUrl(src);
    if (not srcUrl.isLocalFile())
        return srcUrl.url();

    QFileInfo fi(srcUrl.toLocalFile());

    QDir st(store);
    if (not st.exists())
        st.mkpath(st.absolutePath());
    QString destFilePath = store + QDir::separator() + fi.fileName();
    qDebug() << Q_FUNC_INFO << destFilePath;
    bool result = QFile::copy(fi.filePath(), destFilePath);
    if (not result)
    {
        qWarning() << "Failed to copy to Store:" << store;
    }

    return QUrl::fromLocalFile(destFilePath).toString();
}

void purge_store_cache(QString store)
{
    qDebug() << Q_FUNC_INFO << "Store:" << store;
    QDir st(store);
    if (st.exists())
    {
        qDebug() << Q_FUNC_INFO << "Store exists";
        st.removeRecursively();
    }
}

}
