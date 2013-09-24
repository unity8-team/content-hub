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
#ifndef COM_UBUNTU_CONTENT_STORE_H_
#define COM_UBUNTU_CONTENT_STORE_H_

#include <QObject>
#include <QSharedPointer>

namespace com
{
namespace ubuntu
{
namespace content
{
class Store : public QObject
{
    Q_OBJECT
  public:
    Q_PROPERTY(QString uri READ uri)

    Store(const QString& uri, QObject* parent = nullptr);
    Store(const Store&);
    virtual ~Store();
    
    Store& operator=(const Store&);
    
    Q_INVOKABLE virtual const QString& uri() const;

  protected:
    struct Private;
    QSharedPointer<Private> d;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_STORE_H_
