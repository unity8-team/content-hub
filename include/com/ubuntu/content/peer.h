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
#ifndef COM_UBUNTU_CONTENT_PEER_H_
#define COM_UBUNTU_CONTENT_PEER_H_

#include <QtDBus>
#include <QObject>
#include <QSharedPointer>
#include <QImage>

namespace com
{
namespace ubuntu
{
namespace content
{
class Peer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QImage icon READ icon WRITE setIcon)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName)

  public:
    static const Peer& unknown();
    Peer(const QString& id = QString(), QObject* parent = nullptr);
    Peer(const Peer& rhs);
    virtual ~Peer();
    
    Peer& operator=(const Peer& rhs);
    bool operator==(const Peer& rhs) const;

    Q_INVOKABLE virtual const QString& id() const;
    Q_INVOKABLE virtual QString name() const;
    Q_INVOKABLE void setName(const QString&);
    Q_INVOKABLE virtual QImage icon() const;
    Q_INVOKABLE void setIcon(const QImage&);
    Q_INVOKABLE virtual QString iconName() const;
    Q_INVOKABLE void setIconName(const QString&);

  private:
    struct Private;
    QSharedPointer<Private> d;
};
}
}
}

Q_DECL_EXPORT
QDBusArgument &operator<<(QDBusArgument &argument,
                const com::ubuntu::content::Peer &peer);

Q_DECL_EXPORT
const QDBusArgument &operator>>(const QDBusArgument &argument,
                com::ubuntu::content::Peer &peer);

Q_DECLARE_METATYPE(com::ubuntu::content::Peer)

#endif // COM_UBUNTU_CONTENT_PEER_H_
