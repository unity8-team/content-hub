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

#include <com/ubuntu/content/peer.h>
#include <QMetaType>
#include "debug.h"
#include "utils.cpp"

namespace cuc = com::ubuntu::content;
namespace ual = ubuntu::app_launch;

struct cuc::Peer::Private
{
    Private (QString id, bool isDefaultPeer, bool legacy) : id(id), isDefaultPeer(isDefaultPeer), legacy(legacy)
    {
        TRACE() << Q_FUNC_INFO << id;
        if (not id.isEmpty()) {
            TRACE() << Q_FUNC_INFO << "Getting appinfo for" << id;

            QString iconPath = icon_path_for_app_id(id);
            qWarning() << Q_FUNC_INFO << "iconPath:" << iconPath;
            if (QFile::exists(iconPath)) {
                QFile iconFile(iconPath);
                if(iconFile.open(QIODevice::ReadOnly)) {
                    iconData = iconFile.readAll();
                    iconFile.close();
                }
            }
        }
    }

    Private (QString id, QString name, QByteArray iconData, QString iconName, bool isDefaultPeer, bool legacy) : id(id), name(name), iconData(iconData), iconName(iconName), isDefaultPeer(isDefaultPeer), legacy(legacy)
    {
        TRACE() << Q_FUNC_INFO << id;
    }

    QString id;
    QString name;
    QByteArray iconData;
    QString iconName;
    bool isDefaultPeer;
    bool legacy;
};

const cuc::Peer& cuc::Peer::unknown()
{
    static cuc::Peer peer;
    return peer;
}

cuc::Peer::Peer(const QString& id, bool isDefaultPeer, bool legacy, QObject* parent) : QObject(parent), d(new cuc::Peer::Private{id, isDefaultPeer, legacy})
{
    TRACE() << Q_FUNC_INFO;
}

cuc::Peer::Peer(const QString& id, const QString& name, QByteArray& iconData, const QString& iconName, bool isDefaultPeer, bool legacy, QObject* parent) : QObject(parent), d(new cuc::Peer::Private{id, name, iconData, iconName, isDefaultPeer, legacy})
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

bool cuc::Peer::legacy() const
{
    return d->legacy;
}

QDBusArgument &operator<<(QDBusArgument &argument, const cuc::Peer& peer)
{
    argument.beginStructure();
    argument << peer.id() << peer.name() << peer.iconData() << peer.iconName() << peer.isDefaultPeer() << peer.legacy();
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
    bool legacy;

    argument.beginStructure();
    argument >> id >> name >> ic >> iconName >> isDefaultPeer >> legacy;
    argument.endStructure();

    peer = cuc::Peer{id, name, ic, iconName, isDefaultPeer, legacy};
    return argument;
}
