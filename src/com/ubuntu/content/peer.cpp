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

#include <gio/gdesktopappinfo.h>
#include <ubuntu-app-launch.h>
#include <com/ubuntu/content/peer.h>
#include <QMetaType>
#include "debug.h"

namespace cuc = com::ubuntu::content;

struct cuc::Peer::Private
{
    Private (QString id, bool isDefaultPeer) : id(id), isDefaultPeer(isDefaultPeer)
    {
        TRACE() << Q_FUNC_INFO << id;
        if (not id.isEmpty()) {
            TRACE() << Q_FUNC_INFO << "Getting appinfo for" << id;
            char * dir = nullptr;
            char * file = nullptr;
            GDesktopAppInfo* app;

            if (ubuntu_app_launch_application_info(id.toStdString().c_str(), &dir, &file)) {
                TRACE() << "DIR:" << QString::fromUtf8(dir);
                TRACE() << "FILE:" << QString::fromUtf8(file);
                TRACE() << "PATH:" << QString::fromUtf8(g_strjoin("/", dir, file, NULL));
                app = g_desktop_app_info_new_from_filename (g_strjoin("/", dir, file, NULL));
            } else {
                QString desktop_id(id + ".desktop");
                app = g_desktop_app_info_new(desktop_id.toLocal8Bit().data());
            }
            if (G_IS_APP_INFO(app)) {
                TRACE() << Q_FUNC_INFO << "Has appinfo";
                name = QString::fromUtf8(g_app_info_get_display_name(G_APP_INFO(app)));
                TRACE() << Q_FUNC_INFO << "name:" << name;
                GIcon* ic = g_app_info_get_icon(G_APP_INFO(app));
                if (G_IS_ICON(ic))
                {
                    iconName = QString::fromUtf8(g_icon_to_string(ic));
                    if (QFile::exists(iconName)) {
                        QFile iconFile(iconName);
                        if(iconFile.open(QIODevice::ReadOnly)) {
                            iconData = iconFile.readAll();
                            iconFile.close();
                        }
                    }
                }
                g_object_unref(app);
            } else {
                TRACE() << Q_FUNC_INFO << "No appinfo, falling back to keyfile";
                GKeyFile *key_file = g_key_file_new();
                GError *error = NULL;
                if (!g_key_file_load_from_file(key_file,
                                               g_strjoin("/", dir, file, NULL),
                                               G_KEY_FILE_NONE,
                                               &error)) {
                    qWarning() << "ERROR:" <<error->message;
                } else {
                    name = QString::fromUtf8 (g_key_file_get_locale_string(key_file,
                                                                           "Desktop Entry",
                                                                           "Name",
                                                                           NULL,
                                                                           &error));
                    TRACE() << Q_FUNC_INFO << "name:" << name;
                    iconName = QString::fromUtf8 (g_key_file_get_locale_string(key_file,
                                                                               "Desktop Entry",
                                                                               "Icon",
                                                                               NULL,
                                                                               &error));
                    TRACE() << Q_FUNC_INFO << "iconName:" << iconName;
                    if (QFile::exists(QString::fromUtf8 (dir) + "/" + iconName)) {
                        QFile iconFile(QString::fromUtf8 (dir) + "/" + iconName);
                        if(iconFile.open(QIODevice::ReadOnly)) {
                            iconData = iconFile.readAll();
                            iconFile.close();
                        }
                    }
                }
            }
            g_free(dir);
            g_free(file);
        }
    }

    Private (QString id, QString name, QByteArray iconData, QString iconName, bool isDefaultPeer) : id(id), name(name), iconData(iconData), iconName(iconName), isDefaultPeer(isDefaultPeer)
    {
        TRACE() << Q_FUNC_INFO << id;
    }

    QString id;
    QString name;
    QByteArray iconData;
    QString iconName;
    bool isDefaultPeer;
};

const cuc::Peer& cuc::Peer::unknown()
{
    static cuc::Peer peer;
    return peer;
}

cuc::Peer::Peer(const QString& id, bool isDefaultPeer, QObject* parent) : QObject(parent), d(new cuc::Peer::Private{id, isDefaultPeer})
{
    TRACE() << Q_FUNC_INFO;
}

cuc::Peer::Peer(const QString& id, const QString& name, QByteArray& iconData, const QString& iconName, bool isDefaultPeer, QObject* parent) : QObject(parent), d(new cuc::Peer::Private{id, name, iconData, iconName, isDefaultPeer})
{
    TRACE() << Q_FUNC_INFO;
}

cuc::Peer::Peer(const cuc::Peer& rhs) : QObject(rhs.parent()), d(rhs.d)
{
}

cuc::Peer::~Peer()
{
}

cuc::Peer& cuc::Peer::operator=(const cuc::Peer& rhs)
{
    d = rhs.d;    
    return *this;
}

bool cuc::Peer::operator==(const cuc::Peer& rhs) const
{
    if (d == rhs.d)
        return true;

    return d->id == rhs.d->id;
}

const QString& cuc::Peer::id() const
{
    return d->id;
}

QString cuc::Peer::name() const
{
    return d->name;
}

void cuc::Peer::setName(const QString& name)
{
    if (name != d->name)
        d->name = name;
}

QByteArray cuc::Peer::iconData() const
{
    return d->iconData;
}

void cuc::Peer::setIconData(const QByteArray& iconData)
{
    if (iconData != d->iconData)
        d->iconData = iconData;
}

QString cuc::Peer::iconName() const
{
    return d->iconName;
}

void cuc::Peer::setIconName(const QString& iconName)
{
    if (iconName != d->iconName)
        d->iconName = iconName;
}

bool cuc::Peer::isDefaultPeer() const
{
    return d->isDefaultPeer;
}

QDBusArgument &operator<<(QDBusArgument &argument, const cuc::Peer& peer)
{
    argument.beginStructure();
    argument << peer.id() << peer.name() << peer.iconData() << peer.iconName() << peer.isDefaultPeer();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, cuc::Peer &peer)
{
    TRACE() << Q_FUNC_INFO;
    QString id;
    QString name;
    QByteArray ic;
    QString iconName;
    bool isDefaultPeer;

    argument.beginStructure();
    argument >> id >> name >> ic >> iconName >> isDefaultPeer;
    argument.endStructure();

    peer = cuc::Peer{id, name, ic, iconName, isDefaultPeer};
    return argument;
}
