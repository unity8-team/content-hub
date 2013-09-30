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
    Q_PROPERTY(int State READ State NOTIFY StateChanged)
    Q_PROPERTY(QString Store READ Store WRITE SetStore NOTIFY StoreChanged)
    Q_PROPERTY(int SelectionType READ SelectionType WRITE SetSelectionType NOTIFY SelectionTypeChanged)
    Q_PROPERTY(int id READ Id)
    Q_PROPERTY(QString source READ source)
    Q_PROPERTY(QString destination READ destination)

  public:
    Transfer(const int, const QString&, const QString&, QObject* parent = nullptr);
    Transfer(const Transfer&) = delete;
    virtual ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

  Q_SIGNALS:
    void StateChanged(int State);
    void StoreChanged(QString Store);
    void SelectionTypeChanged(int SelectionType);

  public Q_SLOTS:
    int State();
    void Start();
    void Handled();
    void Charge(const QStringList&);
    QStringList Collect();
    void Abort();
    void Finalize();
    QString Store();
    void SetStore(QString);
    int SelectionType();
    void SetSelectionType(int);
    int Id();
    QString source();
    QString destination();
    QString export_path();
    QString import_path();

  private:
    struct Private;
    QScopedPointer<Private> d;

};
}
}
}
}

#endif // TRANSFER_H_
