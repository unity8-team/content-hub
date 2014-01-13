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
#ifndef SHARE_HANDLER_H_
#define SHARE_HANDLER_H_

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>

#include <com/ubuntu/content/share_handler.h>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{

class ShareHandler : public com::ubuntu::content::Handler
{
    Q_OBJECT
  public:
    ShareHandler(QDBusConnection connection,
            const QString& peer_id,
            com::ubuntu::content::ShareHandler *handler = nullptr);
    ShareHandler(const ShareHandler&) = delete;
    ~ShareHandler();

    ShareHandler& operator=(const ShareHandler&) = delete;

  public Q_SLOTS:
    void HandleShare(const QDBusObjectPath &transfer);

  private:
    struct Private;
    QScopedPointer<Private> d;
    com::ubuntu::content::ShareHandler *m_handler;

};
}
}
}
}

#endif // SHARE_HANDLER_H_
