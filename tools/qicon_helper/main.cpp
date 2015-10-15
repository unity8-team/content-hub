/*
 * Copyright (C) 2015 Canonical, Ltd.
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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#include <QGuiApplication>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QIcon>
#include <iostream>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    QIcon icon = QIcon::fromTheme(QString(argv[1]));
    QImage image(icon.pixmap(256,256).toImage());
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");
    QString iconBase64 = QString::fromLatin1(byteArray.toBase64().data());
    std::cout << iconBase64.toStdString();
    a.quit();
}
