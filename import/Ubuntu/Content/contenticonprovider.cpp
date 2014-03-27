/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../../src/com/ubuntu/content/debug.h"
#include "contenticonprovider.h"

ContentIconProvider::ContentIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
    TRACE() << Q_FUNC_INFO;

    appIdImageMap = new QMap<QString, QImage>();
}

ContentIconProvider *ContentIconProvider::instance()
{
    static ContentIconProvider *contentIconProvider = new ContentIconProvider();
    return contentIconProvider;
}

/*!
 * \brief void ContentIconProvider::addImage
 *
 * Add an app's icon to the provider
 */
void ContentIconProvider::addImage(QString appId, QImage image)
{
    TRACE() << Q_FUNC_INFO;
    appIdImageMap->insert(appId, image);
}

/*!
 * \brief QImage ContentIconProvider::requestImage
 *
 * Returns the QImage for a given appId at the requested size
 */
QImage ContentIconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize)
    TRACE() << Q_FUNC_INFO;

    QImage image = appIdImageMap->value(id);
    if(size) {
        *size = image.size();
    }

    return image;
}

