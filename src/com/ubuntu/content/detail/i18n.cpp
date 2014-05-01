/*
 * Copyright © 2014 Canonical Ltd.
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
 * Authored by: Ken VanDine <ken.vandine@canonical.com>
 */

#define NO_TR_OVERRIDE
#include "i18n.h"

#include <libintl.h>

namespace com
{
namespace ubuntu
{
namespace content
{
namespace detail
{

void initTr(const char *domain, const char *localeDir)
{
    bindtextdomain(domain, localeDir);
    textdomain(domain);
}

QString _(const char *text, const char *domain)
{
    return QString::fromUtf8(dgettext(domain, text));
}

}
}
}
}
