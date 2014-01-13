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
#ifndef COM_UBUNTU_CONTENT_HANDLER_H_
#define COM_UBUNTU_CONTENT_HANDLER_H_

#include <QObject>

namespace com
{
namespace ubuntu
{
namespace content
{
class Transfer;

class Handler : public QObject
{
    Q_OBJECT
  public:
    Handler(const Handler&) = delete;
    virtual ~Handler() = default;
    Handler& operator=(const Handler&) = delete;

  protected:
    Handler(QObject* parent = nullptr); 
};
}
}
}

#endif // COM_UBUNTU_CONTENT_HANDLER_H_
