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
#ifndef TRANSFER_H_
#define TRANSFER_H_

#include <QObject>
#include <QStringList>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{
class Transfer : public QObject
{
    Q_OBJECT
  public:
    Transfer(QObject* parent = nullptr);
    Transfer(const Transfer&) = delete;
    virtual ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

  public Q_SLOTS:
    void Abort();
    void Start();
    void Charge(const QStringList&);
    QStringList Collect();

  private:
    struct Private;
    QScopedPointer<Private> d;
};
}
}
}
}

#endif // TRANSFER_H_
