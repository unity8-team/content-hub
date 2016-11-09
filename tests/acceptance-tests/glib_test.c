/*
 * Copyright © 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
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
 * Authored by: William Hua <william.hua@canonical.com>
 */

#include <string.h>
#include <gio/gio.h>

#include "glib/content-hub-glib.h"

static const gchar *SERVICE_NAME = "com.ubuntu.content.dbus.Service";
static const gchar *SERVICE_PATH = "/";
static const gchar *APPLICATION_ID = "com.ubuntu.content.glib_test";
static const gchar *HANDLER_PATH = "/com/ubuntu/content/handler";
static const gchar *SURFACE_ID = "01234567-0123-0123-0123-0123456789ab";

static const gchar * const PASTE_FORMATS[] =
{
  "text/plain",
  "text/html",
  NULL
};

static const gint N_PASTE_FORMATS = sizeof (PASTE_FORMATS) / sizeof (PASTE_FORMATS[0]) - 1;

static const gchar PASTE_DATA[] =
  "text/plain"
  "plain text"
  "text/html"
  "<p>html text</p>";

static const gint N_PASTE_DATA = sizeof (PASTE_DATA) / sizeof (PASTE_DATA[0]) - 1;

static gboolean pasteboard_changed;

static void
pasteboard_changed_cb (ContentHubService *service,
                       gpointer           user_data)
{
  GMainLoop *main_loop = user_data;

  pasteboard_changed = TRUE;

  g_main_loop_quit (main_loop);
}

static gboolean
timed_out_cb (gpointer user_data)
{
  GMainLoop *main_loop = user_data;

  g_main_loop_quit (main_loop);

  return G_SOURCE_CONTINUE;
}

static void
set_paste (ContentHubService *service)
{
  GMainLoop *main_loop;
  guint timeout_id;

  g_assert_false (pasteboard_changed);

  main_loop = g_main_loop_new (NULL, FALSE);
  timeout_id = g_timeout_add (1000, timed_out_cb, main_loop);

  g_signal_connect (
    service,
    "pasteboard-changed",
    G_CALLBACK (pasteboard_changed_cb),
    main_loop);

  g_assert_true (
    content_hub_service_call_create_paste_sync (
      service,
      APPLICATION_ID,
      SURFACE_ID,
      g_variant_new_fixed_array (G_VARIANT_TYPE_BYTE, PASTE_DATA, sizeof (PASTE_DATA) - 1, sizeof (guchar)),
      PASTE_FORMATS,
      NULL,
      NULL,
      NULL));

  g_main_loop_run (main_loop);
  g_source_remove (timeout_id);
  g_clear_pointer (&main_loop, g_main_loop_unref);

  g_assert_true (pasteboard_changed);

  pasteboard_changed = FALSE;
}

static void
get_paste (ContentHubService *service)
{
  gchar **formats;
  GVariant *variant;
  gconstpointer data;
  gsize size;
  gint i;

  g_assert_true (
    content_hub_service_call_paste_formats_sync (
      service,
      &formats,
      NULL,
      NULL));

  g_assert_cmpint (g_strv_length (formats), ==, N_PASTE_FORMATS);

  for (i = 0; i < N_PASTE_FORMATS; i++)
    g_assert_cmpstr (formats[i], ==, PASTE_FORMATS[i]);

  g_strfreev (formats);

  g_assert_true (
    content_hub_service_call_get_latest_paste_data_sync (
      service,
      SURFACE_ID,
      &variant,
      NULL,
      NULL));

  data = g_variant_get_fixed_array (variant, &size, sizeof (guchar));

  g_assert_cmpint (size, ==, N_PASTE_DATA);
  g_assert_cmpmem (data, size, PASTE_DATA, N_PASTE_DATA);
}

int
main (int   argc,
      char *argv[])
{
  GDBusConnection *session;
  ContentHubService *service;
  ContentHubHandler *handler;

  session = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);

  service = content_hub_service_proxy_new_sync (
    session,
    G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
    SERVICE_NAME,
    SERVICE_PATH,
    NULL,
    NULL);

  g_assert_nonnull (service);

  handler = content_hub_handler_skeleton_new ();

  g_assert_true (
    g_dbus_interface_skeleton_export (
      G_DBUS_INTERFACE_SKELETON (handler),
      session,
      HANDLER_PATH,
      NULL));

  g_object_unref (session);

  g_assert_true (
    content_hub_service_call_register_import_export_handler_sync (
      service,
      APPLICATION_ID,
      HANDLER_PATH,
      NULL,
      NULL));

  g_assert_true (
    content_hub_service_call_handler_active_sync (
      service,
      APPLICATION_ID,
      NULL,
      NULL));

  set_paste (service);
  get_paste (service);

  g_dbus_interface_skeleton_unexport (G_DBUS_INTERFACE_SKELETON (handler));
  g_object_unref (handler);
  g_object_unref (service);

  return 0;
}
