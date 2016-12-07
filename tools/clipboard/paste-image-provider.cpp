/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "paste-image-provider.h"

#include <QDebug>
#include <QMimeData>

#include "paste-data-provider.h"

PasteImageProvider::PasteImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap),
    m_provider(new PasteDataProvider())
{
}

PasteImageProvider::~PasteImageProvider()
{
}

QImage PasteImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "IMAGE PROVIDER" << id;
    QString surfaceId = "foobar";
    QMimeData *mimeData = m_provider->pasteDataById(surfaceId, id.toInt());
    if (!mimeData || !mimeData->hasImage()) {
        return QImage();
    }

    QImage image = qvariant_cast<QImage>(mimeData->imageData());
    if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        return image.scaled(requestedSize.width(), requestedSize.height(), Qt::IgnoreAspectRatio);
    }

    return image;
}
