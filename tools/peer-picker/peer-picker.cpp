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
#include "peer-picker.h"

PeerPicker::PeerPicker(int& argc, char** argv)
    : QApplication(argc, argv),
      m_view(new QQuickView())
{
    bool fullScreen = false;
    QString requesterId;
    QString contentType;
    QString handlerType;

    QStringList args = arguments();

    if (args.contains("--fullscreen")) {
        args.removeAll("--fullscreen");
        fullScreen = true;
    }

    if (args.count() < 4) {
        std::cout << "Usage: content-hub-peer-picker app_id content_type handler_type\n";
        QApplication::exit(1);
    } else {
        requesterId = args.at(1);
        contentType = args.at(2);
        handlerType = args.at(3);
    }

    // Set context properties needed by the picker
    m_view->rootContext()->setContextProperty("requesterId", requesterId);
    m_view->rootContext()->setContextProperty("wellKnownType", contentType);
    m_view->rootContext()->setContextProperty("handlerType", handlerType);
    m_view->setSource(QUrl(QStringLiteral("qrc:/main.qml")));
    if (fullScreen) {
        m_view->showFullScreen();
    } else {
        m_view->show();
    }
}

int PeerPicker::exec()
{
    return QApplication::exec();
}

PeerPicker::~PeerPicker()
{
    delete m_view;
}
