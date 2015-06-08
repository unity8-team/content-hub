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

#include <com/ubuntu/content/type.h>

#include <QCoreApplication>

namespace cuc = com::ubuntu::content;

struct cuc::Type::Private
{
    QString id;
};

cuc::Type::Type(const QString& id, QObject* parent) : QObject(parent), d(new Private{id})
{
}

cuc::Type::Type(const cuc::Type& rhs) : QObject(rhs.parent()), d(rhs.d)
{
}

cuc::Type::~Type()
{
}

cuc::Type& cuc::Type::operator=(const cuc::Type& rhs)
{
    d = rhs.d;
    return *this;
}

bool cuc::Type::operator==(const cuc::Type& rhs) const
{
    if (d == rhs.d)
        return true;

    return d->id == rhs.d->id;
}

bool cuc::Type::operator<(const cuc::Type& rhs) const
{
    return d->id < rhs.d->id;
}

const QString& cuc::Type::id() const
{
    return d->id;
}

const cuc::Type& cuc::Type::unknown()
{
    static cuc::Type t("unknown", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::all()
{
    static cuc::Type t("all", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::documents()
{
    static cuc::Type t("documents", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::pictures()
{
    static cuc::Type t("pictures", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::music()
{
    static cuc::Type t("music", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::contacts()
{
    static cuc::Type t("contacts", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::videos()
{
    static cuc::Type t("videos", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::links()
{
    static cuc::Type t("links", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::ebooks()
{
    static cuc::Type t("ebooks", nullptr);
    return t;
}

const cuc::Type& cuc::Type::Known::text()
{
    static cuc::Type t("text", nullptr);
    return t;
}
