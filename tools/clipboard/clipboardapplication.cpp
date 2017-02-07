/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Arthur Mello <arthur.mello@canonical.com>
 */

#include "clipboardapplication.h"

#include <iostream>

#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include <qpa/qplatformnativeinterface.h>
#include <mir_toolkit/mir_surface.h>

#include "debug.h"
#include "paste-data-model.h"
#include "paste-data-filter-model.h"
#include "paste-image-provider.h"

ClipboardApplication::ClipboardApplication(int &argc, char **argv)
    : QApplication(argc, argv),
    m_surfaceId(),
    m_view(new QQuickView())
{
    connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)), SLOT(onApplicationStateChanged(Qt::ApplicationState)));

    bool fullScreen = false;
    QString requesterId;

    QStringList args = arguments();
    if (args.contains("--fullscreen")) {
        args.removeAll("--fullscreen");
        fullScreen = true;
    }

    if (args.count() < 2) {
        std::cout << "Usage: content-hub-peer-clipboard app_id\n";
        QApplication::exit(1);
    } else {
        requesterId = args.at(1);
    }

    const char* uri = "clipboardapp.private";
    qmlRegisterType<PasteDataModel>(uri, 0, 1, "PasteDataModel");
    qmlRegisterType<PasteDataFilterModel>(uri, 0, 1, "PasteDataFilterModel");

    QObject::connect(m_view->engine(), SIGNAL(quit()), SLOT(quit()));
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->engine()->addImageProvider(QLatin1String("pastedImage"), new PasteImageProvider);
    m_view->rootContext()->setContextProperty("application", this);
    m_view->rootContext()->setContextProperty("requesterId", requesterId);
    m_view->setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    if (fullScreen) {
        m_view->showFullScreen();
    } else {
        m_view->show();
    }
    m_view->requestActivate();
}

ClipboardApplication::~ClipboardApplication()
{
    delete m_view;
}

int ClipboardApplication::exec()
{
    return QApplication::exec();
}

const QString& ClipboardApplication::surfaceId() const
{
    return m_surfaceId;
}

int ClipboardApplication::appState() const
{
    return applicationState();
}

void ClipboardApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
    Q_EMIT(appStateChanged());

    if (m_surfaceId.isEmpty() && state == Qt::ApplicationActive) {
        m_surfaceId = requestSurfaceId();
        if (m_surfaceId.isEmpty()) {
            TRACE() << Q_FUNC_INFO << "Unable to request MIR surfaceId. Clipboard will not be able to get any data from content-hub";
        } else {
            Q_EMIT(surfaceIdChanged());
        }
    }
}

QString ClipboardApplication::requestSurfaceId()
{
    auto mirSurface = static_cast<MirSurface*>(platformNativeInterface()->nativeResourceForWindow("mirsurface", m_view));
    if (!mirSurface)
        return QString();

    MirPersistentId* mirPermaId = mir_surface_request_persistent_id_sync(mirSurface);
    QString surfaceId(mir_persistent_id_as_string(mirPermaId));
    mir_persistent_id_release(mirPermaId);

    return surfaceId;
}
