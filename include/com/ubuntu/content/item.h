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

#include <QtDBus>
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
    Q_PROPERTY(QUrl url READ url() WRITE setUrl)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QByteArray stream READ stream WRITE setStream)
    Q_PROPERTY(QString streamType READ streamType WRITE setStreamType)

  public:
    Item(const QUrl& = QUrl(), QObject* = nullptr);
    Item(const Item&);
    virtual ~Item();

    Item& operator=(const Item&);
    bool operator==(const Item&) const;

    Q_INVOKABLE const QUrl& url() const;
    Q_INVOKABLE void setUrl(const QUrl &url) const;
    Q_INVOKABLE const QString& name() const;
    Q_INVOKABLE void setName(const QString &name) const;
    Q_INVOKABLE const QString text() const;
    Q_INVOKABLE void setText(const QString &text) const;
    Q_INVOKABLE const QByteArray& stream() const;
    Q_INVOKABLE void setStream(const QByteArray &stream) const;
    Q_INVOKABLE const QString& streamType() const;
    Q_INVOKABLE void setStreamType(const QString &type) const;

  private:
    struct Private;
    QSharedPointer<Private> d;
};

}
}
}

Q_DECL_EXPORT
QDBusArgument &operator<<(QDBusArgument &argument,
                const com::ubuntu::content::Item &item);

Q_DECL_EXPORT
const QDBusArgument &operator>>(const QDBusArgument &argument,
                com::ubuntu::content::Item &item);

Q_DECLARE_METATYPE(com::ubuntu::content::Item)


#endif // COM_UBUNTU_CONTENT_ITEM_H_
