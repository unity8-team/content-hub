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

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QtCore>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QUrl>
#include <nih/alloc.h>
#include <nih-dbus/dbus_util.h>

#include "common.h"
#include "debug.h"
#include "com/ubuntu/content/type.h"
#include <unistd.h>

#include <sys/apparmor.h>
/* need to be exposed in libapparmor but for now ... */
#define AA_CLASS_FILE 2
#define AA_MAY_READ (1 << 2)

namespace cuc = com::ubuntu::content;

namespace {

QList<cuc::Type> known_types()
{
    QList<cuc::Type> types;
    types << cuc::Type::Known::pictures();
    types << cuc::Type::Known::music();
    types << cuc::Type::Known::documents();
    types << cuc::Type::Known::contacts();
    return types;
}

/* sanitize the dbus names */
QString sanitize_id(const QString& appId)
{
    TRACE() << Q_FUNC_INFO;
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
    TRACE() << Q_FUNC_INFO << uniqueConnectionId;
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
        TRACE() << "AppArmor Profile:" << aaProfile;
    } else {
        qWarning() << "Error getting app ID:" << reply.errorName() <<
            reply.errorMessage();
    }

    return aaProfile;
}

bool is_persistent(QString store)
{
    TRACE() << Q_FUNC_INFO << store;
    QRegExp rx("*.cache/*/HubIncoming/*");
    rx.setPatternSyntax(QRegExp::Wildcard);
    rx.setCaseSensitivity(Qt::CaseSensitive);
    return not rx.exactMatch(store);
}

QString copy_to_store(const QString& src, const QString& store)
{
    TRACE() << Q_FUNC_INFO;
    QUrl srcUrl(src);
    if (not srcUrl.isLocalFile())
        return srcUrl.url();

    QFileInfo fi(srcUrl.toLocalFile());

    QDir st(store);
    if (not st.exists())
        st.mkpath(st.absolutePath());
    QString destFilePath = store + QDir::separator() + fi.fileName();
    TRACE() << Q_FUNC_INFO << destFilePath;
    if (QFile::exists(destFilePath)) {
            qWarning() << "Destination file already exists, aborting:" << destFilePath;
            return QString();
    }
    bool copy_failed = true;
    if (not is_persistent(store))
    {
        if (link( fi.absoluteFilePath().toStdString().c_str(),
            destFilePath.toStdString().c_str() ) < 0)
        {
            qWarning() << "Can't create hard link to Store:" << store;
        } else
            copy_failed = false;
    }
    if (copy_failed)
    {
        if (not QFile::copy(fi.filePath(), destFilePath))
            qWarning() << "Failed to copy to Store:" << store;
    }

    return QUrl::fromLocalFile(destFilePath).toString();
}

bool purge_store_cache(QString store)
{
    TRACE() << Q_FUNC_INFO << "Store:" << store;

    if (is_persistent(store))
    {
        TRACE() << Q_FUNC_INFO << store << "is persistent";
        return false;
    }

    QDir st(store);
    if (st.exists())
    {
        TRACE() << Q_FUNC_INFO << store << "isn't persistent, purging";
        return st.removeRecursively();
    }

    return false;
}

int query_file(const char *label, const char *path, int *allowed)
{
    int rc, audited;
    char *query;

    /* + 1 for null separator and then + 1 AA_CLASS_FILE */
    int label_size = strlen(label);
    int size = label_size + 1 + strlen(path) + AA_QUERY_CMD_LABEL_SIZE + 1;
    /* +1 for null terminator used by strcpy, yes we could drop this
     * using memcpy */
    query = (char*)malloc(size + 1);
    if (!query)
        return -1;
    /* we want the null terminator here */
    strcpy(query + AA_QUERY_CMD_LABEL_SIZE, label);
    query[AA_QUERY_CMD_LABEL_SIZE + label_size + 1] = AA_CLASS_FILE;
    strcpy(query + AA_QUERY_CMD_LABEL_SIZE + label_size + 2, path);
    rc = aa_query_label(AA_MAY_READ, query, size , allowed, &audited);
    free(query);
    return rc;
}

bool check_profile_read(QString profile, QString path)
{
    TRACE() << Q_FUNC_INFO << "PROFILE:" << profile;

    // If app is unconfined it has access
    if (profile.toStdString() == QString("unconfined").toStdString())
        return true;

    int allowed;
    if (query_file(profile.toStdString().c_str(), path.toStdString().c_str(), &allowed) == -1) {
        qWarning() << "error:" << strerror(errno) << path;
        return false;
    }
   
    if (allowed) {
        TRACE() << "ALLOWED:" << QString::number(allowed);
        return true;
    }
    TRACE() << "NOT ALLOWED:" << QString::number(allowed);
    return false;

}

}
