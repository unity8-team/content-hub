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
        if (name.isEmpty()) {
            char * dir = nullptr;
            char * file = nullptr;

            GDesktopAppInfo* app;
            if (ubuntu_app_launch_application_info(id.toStdString().c_str(), &dir, &file)) {
                app = g_desktop_app_info_new_from_filename (g_strjoin("/", dir, file, NULL));
            } else {
                QString desktop_id(id + ".desktop");
                app = g_desktop_app_info_new(desktop_id.toLocal8Bit().data());
            }
            if (G_IS_APP_INFO(app)) {
                name = QString::fromUtf8(g_app_info_get_display_name(G_APP_INFO(app)));
                GIcon* ic = g_app_info_get_icon(G_APP_INFO(app));
                if (G_IS_ICON(ic))
                {
                    iconName = QString::fromUtf8(g_icon_to_string(ic));
                    /* Special case for firefox which doesn't have a themed icon */
                    if (iconName == QString("firefox")) {
                        iconName = QString(dir) + "/pixmaps/firefox.png";
                    }
                    if (QFile::exists(iconName)) {
                        QFile iconFile(iconName);
                        if(iconFile.open(QIODevice::ReadOnly)) {
                            iconData = iconFile.readAll();
                            iconFile.close();
                        }
                    }
                }
                g_object_unref(app);
            }
            g_free(dir);
            g_free(file);
        }
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

    peer = cuc::Peer{id, isDefaultPeer};
    peer.setName(name);
    peer.setIconData(ic);
    peer.setIconName(iconName);
    return argument;
}
