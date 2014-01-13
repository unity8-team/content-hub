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
#ifndef COM_UBUNTU_CONTENT_SHARE_HANDLER_H_
#define COM_UBUNTU_CONTENT_SHARE_HANDLER_H_

#include "handler.h"

namespace com
{
namespace ubuntu
{
namespace content
{
class Transfer;

class ShareHandler : public Handler
{
    Q_OBJECT
  public:
    ShareHandler(const ShareHandler&) = delete;
    virtual ~ShareHandler() = default;
    ShareHandler& operator=(const ShareHandler&) = delete;

    Q_INVOKABLE virtual void handle_share(Transfer*) = 0;

  protected:
    ShareHandler(QObject* parent = nullptr);
};
}
}
}

#endif // COM_UBUNTU_CONTENT_SHARE_HANDLER_H_
