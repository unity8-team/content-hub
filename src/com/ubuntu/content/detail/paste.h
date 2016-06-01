/*
 * Copyright © 2016 Canonical Ltd.
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
#ifndef PASTE_H_
#define PASTE_H_

#include <QDir>
#include <QObject>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusContext>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{
class Paste : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_PROPERTY(int State READ State NOTIFY StateChanged)
    Q_PROPERTY(int id READ Id)
    Q_PROPERTY(QString source READ source)

  public:
    Paste(const int, const QString&, QObject* parent = nullptr);
    Paste(const Paste&) = delete;
    virtual ~Paste();

    Paste& operator=(const Paste&) = delete;

Q_SIGNALS:
    void StateChanged(int State);

  public Q_SLOTS:
    int State();
    void Charge(const QVariantList&);
    QVariantList Collect();
    void Abort();
    void Finalize();
    int Id();
    QString source();
    QString destination();
    void setDestination(QString&);
    QString path();

  private:
    struct Private;
    QScopedPointer<Private> d;

};
}
}
}
}

#endif // PASTE_H_
