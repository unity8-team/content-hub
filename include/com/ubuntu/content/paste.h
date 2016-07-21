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
#ifndef COM_UBUNTU_CONTENT_PASTE_H_
#define COM_UBUNTU_CONTENT_PASTE_H_

#include <QObject>
#include <QSharedPointer>
#include <QMimeData>
#include <QString>

namespace com
{
namespace ubuntu
{
namespace content
{

class Paste : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString source READ source)

  public:
    enum State
    {
        created,
        charged,
        saved,
        collected
    };

    Paste(const Paste&) = delete;
    virtual ~Paste();

    Paste& operator=(const Paste&) = delete;

    Q_INVOKABLE virtual int id() const;
    Q_INVOKABLE virtual State state() const;
    Q_INVOKABLE virtual QMimeData* mimeData();
    Q_INVOKABLE virtual QString source() const;

    Q_SIGNAL void stateChanged();

  private:
    struct Private;
    friend struct Private;
    friend class Hub;
    QSharedPointer<Private> d;

    Paste(const QSharedPointer<Private>&, QObject* parent = nullptr);    
};
}
}
}

#endif // COM_UBUNTU_CONTENT_PASTE_H_
