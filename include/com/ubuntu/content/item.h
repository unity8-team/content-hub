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
#ifndef COM_UBUNTU_CONTENT_ITEM_H_
#define COM_UBUNTU_CONTENT_ITEM_H_

#include <QObject>
#include <QSharedPointer>
#include <QUrl>

namespace com
{
namespace ubuntu
{
namespace content
{
class Item : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url())

  public:
    Item(const QUrl& = QUrl(), QObject* = nullptr);
    Item(const Item&);
    virtual ~Item();

    Item& operator=(const Item&);
    bool operator==(const Item&) const;

    Q_INVOKABLE const QUrl& url() const;

  private:
    struct Private;
    QSharedPointer<Private> d;
};
}
}
}

#endif // COM_UBUNTU_CONTENT_ITEM_H_
