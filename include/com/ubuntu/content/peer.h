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

#include <QObject>
#include <QSharedPointer>

namespace com
{
namespace ubuntu
{
namespace content
{
class Peer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id())

  public:
    static const Peer& unknown();

    Peer(const QString& id = QString(), QObject* parent = nullptr);
    Peer(const Peer& rhs);
    virtual ~Peer();
    
    Peer& operator=(const Peer& rhs);
    bool operator==(const Peer& rhs) const;

    Q_INVOKABLE virtual const QString& id() const;

  private:
    struct Private;
    QSharedPointer<Private> d;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_PEER_H_
