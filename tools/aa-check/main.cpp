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

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QString>

#include <sys/apparmor.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::string profile, path;
    profile = a.arguments().at(1).toStdString(); 
    path = a.arguments().at(2).toStdString(); 

    if (aa_change_profile(profile.c_str()) != 0) {
        qWarning() << "Failed to check profile:" << profile.c_str();
        return 1;
    }

    QFile file(QString::fromStdString(path));
    if (file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can access file";
        return 0;
    } else {
        qWarning() << "Can't access file";
        return 1;
    }

    file.close();

    return a.exec();
}
