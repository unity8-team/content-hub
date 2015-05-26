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

#include "debug.h"
#include "transfer.h"
#include "utils.cpp"

#include <QFileInfo>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/store.h>
#include <com/ubuntu/content/transfer.h>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/manager.h>

namespace cuc = com::ubuntu::content;
namespace cucd = com::ubuntu::content::detail;

struct cucd::Transfer::Private
{
    Private(const int id,
            const QString& source,
            const QString& destination,
            const int direction,
            const QString& content_type,
            const QString& profile):
        state(cuc::Transfer::created),
            id(id),
            source(source),
            destination(destination),
            direction(direction),
            selection_type(cuc::Transfer::single),
            source_started_by_content_hub(false),
            content_type(content_type),
            profile(profile)
    {
    }
    
    cuc::Transfer::State state;
    const int id;
    const QString source;
    const QString destination;
    int direction;
    QString store;
    int selection_type;
    QVariantList items;
    bool source_started_by_content_hub;
    QString download_id;
    const QString content_type;
    const QString profile;
};

cucd::Transfer::Transfer(const int id,
                         const QString& source,
                         const QString& destination,
                         const int direction,
                         const QString& content_type,
                         const QString& profile,
                         QObject* parent) :
    QObject(parent), d(new Private(id, source, destination, direction, content_type, profile))
{
    TRACE() << __PRETTY_FUNCTION__;
}

cucd::Transfer::~Transfer()
{
    TRACE() << __PRETTY_FUNCTION__;
    purge_store_cache(d->store);
}

/* unique id of the transfer */
int cucd::Transfer::Id()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->id;
}

/* returns the peer_id of the requested export handler */
QString cucd::Transfer::source()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->source;
}

/* returns the peer_id of the application requesting the import */
QString cucd::Transfer::destination()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->destination;
}

int cucd::Transfer::Direction()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->direction;
}

int cucd::Transfer::State()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->state;
}

void cucd::Transfer::Abort()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::aborted)
        return;

    purge_store_cache(d->store);
    d->items.clear();
    d->state = cuc::Transfer::aborted;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Start()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::initiated)
        return;

    d->state = cuc::Transfer::initiated;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Handled()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::in_progress)
        return;

    d->state = cuc::Transfer::in_progress;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Charge(const QVariantList& items)
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::charged)
        return;

    if (d->state == cuc::Transfer::downloading)
    {
        qWarning() << "Unable to charge, download still in progress.";
        return;
    }

    if (d->state == cuc::Transfer::downloaded)
    {
        d->state = cuc::Transfer::charged;
        Q_EMIT(StateChanged(d->state));
        return;
    } 

    QVariantList ret;
    Q_FOREACH(QVariant iv, items) {
        cuc::Item item = qdbus_cast<Item>(iv);
        if (item.url().isEmpty()) {
            ret.append(QVariant::fromValue(item));
        } else {
            TRACE() << Q_FUNC_INFO;
            if (d->profile.toStdString() != QString("unconfined").toStdString() &&
                item.url().isLocalFile()) {
                TRACE() << Q_FUNC_INFO << "IS LOCAL FILE";
                QString file(item.url().toLocalFile());
                TRACE() << Q_FUNC_INFO << "FILE:" << file;
                // Verify app has read access to local file before transfer
                if (not check_profile_read(d->profile, file)) {
                    // If failed to access file, abort
                    ret.clear();
                    goto abort;
                }
            }
            QString newUrl = copy_to_store(item.url().toString(), d->store);
            if (!newUrl.isEmpty()) {
                item.setUrl(QUrl(newUrl));
                TRACE() << Q_FUNC_INFO << "Item:" << item.url();
                ret.append(QVariant::fromValue(item));
            } else {
                ret.clear();
                goto abort;
            }
        }
    }

abort:
    if (ret.count() <= 0)
    {
        qWarning() << "Failed to charge items, aborting";
        d->state = cuc::Transfer::aborted;
    }
    else
    {
        d->items = ret;
        d->state = cuc::Transfer::charged;
    }
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::Download()
{
    TRACE() << __PRETTY_FUNCTION__;
    if(d->download_id.isEmpty()) 
    {
        return;
    }

    Manager *downloadManager = Manager::createSessionManager();
    auto download = downloadManager->getDownloadForId(d->download_id);
    if (download == nullptr) 
    {
        TRACE() << downloadManager->lastError();
    }
    else
    {
        QDir dir;
        dir.mkpath(d->store);
        download->setDestinationDir(d->store);
        connect(download, SIGNAL(finished(QString)), this, SLOT(DownloadComplete(QString)));
        connect(download, SIGNAL(error(Ubuntu::DownloadManager::Error*)), this, SLOT(DownloadError(Ubuntu::DownloadManager::Error*)));
        download->start();
        d->state = cuc::Transfer::downloading;
        Q_EMIT(StateChanged(d->state));
    }
}

void cucd::Transfer::AddItemsFromDir(QDir dir) {
    QFileInfoList files = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    foreach(const QFileInfo &fileInfo, files) {
        QString path = fileInfo.absoluteFilePath();
        if(fileInfo.isDir()) {
            AddItemsFromDir(QDir(path));
        } else {
            cuc::Item item = cuc::Item{QUrl::fromLocalFile(path).toString()};
            d->items.append(QVariant::fromValue(item));
        }
    }
}

void cucd::Transfer::DownloadComplete(QString destFilePath)
{
    TRACE() << __PRETTY_FUNCTION__;
    QFileInfo fileInfo(destFilePath);
    if(fileInfo.isDir()) {
        // When downloading and deflating zip files, download manager may
        // send us the path of the directory that multiple files have been
        // unpacked into.
        AddItemsFromDir(QDir(destFilePath));
    } else {
        cuc::Item item = cuc::Item{QUrl::fromLocalFile(destFilePath).toString()};
        d->items.append(QVariant::fromValue(item));
    }
    d->state = cuc::Transfer::downloaded;
    Q_EMIT(StateChanged(d->state));
}

void cucd::Transfer::DownloadError(Ubuntu::DownloadManager::Error* error)
{
    TRACE() << __PRETTY_FUNCTION__;
    qWarning() << "Download manager error: " << error->errorString();

    d->state = cuc::Transfer::aborted;
    Q_EMIT(DownloadManagerError(error->errorString()));
    Q_EMIT(StateChanged(d->state));
}

QVariantList cucd::Transfer::Collect()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state != cuc::Transfer::collected)
    {
        d->state = cuc::Transfer::collected;
        Q_EMIT(StateChanged(d->state));
    }

    return d->items;
}

void cucd::Transfer::Finalize()
{
    TRACE() << __PRETTY_FUNCTION__;

    if (d->state == cuc::Transfer::finalized)
        return;

    purge_store_cache(d->store);
    d->items.clear();
    d->state = cuc::Transfer::finalized;
    Q_EMIT(StateChanged(d->state));
}

QString cucd::Transfer::Store()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->store;
}

void cucd::Transfer::SetStore(QString uri)
{
    TRACE() << Q_FUNC_INFO;

    if (d->store == uri)
        return;

    d->store = uri;
    Q_EMIT(StoreChanged(d->store));
}

int cucd::Transfer::SelectionType()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->selection_type;
}

void cucd::Transfer::SetSelectionType(int type)
{
    TRACE() << Q_FUNC_INFO;
    if (d->state != cuc::Transfer::created)
        return;
    if (d->selection_type == type)
        return;

    d->selection_type = type;
    Q_EMIT(SelectionTypeChanged(d->selection_type));
}

QString cucd::Transfer::DownloadId()
{
    TRACE() << Q_FUNC_INFO;
    return d->download_id;
}

void cucd::Transfer::SetDownloadId(QString DownloadId)
{
    TRACE() << Q_FUNC_INFO;
    if (d->download_id == DownloadId)
        return;

    d->download_id = DownloadId;
    Q_EMIT(DownloadIdChanged(d->download_id));
}

/* returns the object path for the export */
QString cucd::Transfer::export_path()
{
    TRACE() << Q_FUNC_INFO << "source:" << d->source;
    static const QString exporter_path_pattern{"/transfers/%1/export/%2"};
    QString source = exporter_path_pattern
            .arg(sanitize_id(d->source))
            .arg(d->id);
    return source;
}

/* returns the object path for the import */
QString cucd::Transfer::import_path()
{
    TRACE() << Q_FUNC_INFO << "destination:" << d->destination;
    static const QString importer_path_pattern{"/transfers/%1/import/%2"};
    QString destination = importer_path_pattern
            .arg(sanitize_id(d->destination))
            .arg(d->id);
    return destination;
}

/* sets, if the source app is freshly started by the content hub */
void cucd::Transfer::SetSourceStartedByContentHub(bool started)
{
    d->source_started_by_content_hub = started;
}

/* returns if the source app was started by the content hub */
bool com::ubuntu::content::detail::Transfer::WasSourceStartedByContentHub() const
{
    return d->source_started_by_content_hub;
}

QString cucd::Transfer::ContentType()
{
    TRACE() << __PRETTY_FUNCTION__;
    return d->content_type;
}
