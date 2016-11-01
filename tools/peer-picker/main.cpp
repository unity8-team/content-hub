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
 */

#include <iostream>
#include <QGuiApplication>
#include <QObject>
#include <QQmlContext>
#include <QQuickView>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    bool fullScreen = false;
    QQuickView *view = new QQuickView;
    QString requesterId;
    QString contentType;
    QString handlerType;

    QStringList args = app.arguments();

    if (args.contains("--fullscreen")) {
        args.removeAll("--fullscreen");
        fullScreen = true;
    }

    if (args.count() < 4) {
        std::cout << "Usage: content-hub-peer-picker app_id content_type handler_type\n";
        return 1;
    } else {
        requesterId = args.at(1);
        contentType = args.at(2);
        handlerType = args.at(3);
    }
    // Set context properties needed by the picker
    view->rootContext()->setContextProperty("requesterId", requesterId);
    view->rootContext()->setContextProperty("wellKnownType", contentType);
    view->rootContext()->setContextProperty("handlerType", handlerType);
    // Hook up quit
    QObject::connect((QObject*)view->engine(), SIGNAL(quit()), (QObject*)&app, SLOT(quit()));
    view->setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    if (fullScreen) {
        view->showFullScreen();
    } else {
        view->show();
    }
    return app.exec();
}

