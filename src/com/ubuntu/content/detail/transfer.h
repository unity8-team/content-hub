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

#include "mir-helper.h"
#include <QObject>
#include <QStringList>
#include <QDir>
#include <ubuntu/download_manager/error.h>

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
    Q_PROPERTY(QString DownloadId READ DownloadId WRITE SetDownloadId NOTIFY DownloadIdChanged)
    Q_PROPERTY(int id READ Id)
    Q_PROPERTY(QString source READ source)
    Q_PROPERTY(QString destination READ destination)
    Q_PROPERTY(int direction READ Direction)
    Q_PROPERTY(QString ContentType READ ContentType)
    Q_PROPERTY(QString MirSocket READ MirSocket WRITE SetMirSocket NOTIFY MirSocketChanged)

  public:
    Transfer(const int, const QString&, const QString&, const int, const QString&, QObject* parent = nullptr);
    Transfer(const Transfer&) = delete;
    virtual ~Transfer();

    Transfer& operator=(const Transfer&) = delete;

    void SetSourceStartedByContentHub(bool started);
    bool WasSourceStartedByContentHub() const;

Q_SIGNALS:
    void StateChanged(int State);
    void StoreChanged(QString Store);
    void SelectionTypeChanged(int SelectionType);
    void DownloadIdChanged(QString DownloadId);
    void DownloadManagerError(QString ErrorMessage);
    void MirSocketChanged(QString MirSocket);

  public Q_SLOTS:
    int State();
    void Start();
    void Handled();
    void Charge(const QVariantList&);
    QVariantList Collect();
    void Abort();
    void Finalize();
    QString Store();
    void SetStore(QString);
    int SelectionType();
    void SetSelectionType(int);
    int Id();
    int Direction();
    QString source();
    QString destination();
    QString export_path();
    QString import_path();
    QString DownloadId();
    void SetDownloadId(QString DownloadId);
    void DownloadComplete(QString destFilePath);
    void Download();
    void DownloadError(Ubuntu::DownloadManager::Error* error);
    QString ContentType();
    void AddItemsFromDir(QDir dir);
    QString MirSocket();
    void SetMirSocket(QString MirSocket);
    PromptSessionP PromptSession();
    void SetPromptSession(PromptSessionP promptSession);

  private:
    struct Private;
    QScopedPointer<Private> d;

};
}
}
}
}

#endif // TRANSFER_H_
