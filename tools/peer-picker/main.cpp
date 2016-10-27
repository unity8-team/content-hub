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

#include <QDebug>
#include <QGuiApplication>
#include <QObject>
#include <QQmlContext>
#include <QQuickView>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    if (!app.arguments().count() > 3) {
        qWarning() << "failed";
        return 1;
    }

    QString requesterId = app.arguments()[1];
    QString contentType = app.arguments()[2];
    QString handlerType = app.arguments()[3];

    QQuickView *view = new QQuickView;
    // Set context properties needed by the picker
    view->rootContext()->setContextProperty("requesterId", requesterId);
    view->rootContext()->setContextProperty("wellKnownType", contentType);
    view->rootContext()->setContextProperty("handlerType", handlerType);
    // Hook up quit
    QObject::connect((QObject*)view->engine(), SIGNAL(quit()), (QObject*)&app, SLOT(quit()));
    view->setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    view->show();
    return app.exec();
}

