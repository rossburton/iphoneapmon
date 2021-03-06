/*
 * Copyright (C) 2011 Intel Corp
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this package; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "ap-monitor.h"
#include <locale.h>

static void
on_found (ApMonitor *monitor, const char *name)
{
  g_print ("Found AP '%s'\n", name);
}

static void
on_lost (ApMonitor *monitor, const char *name)
{
  g_print ("Lost AP '%s'\n", name);
}

static char *
icon_to_string (GIcon *icon)
{
  char *s;

  if (icon == NULL)
    return g_strdup ("");
  s = g_icon_to_string (icon);
  g_object_unref (icon);

  return s;
}

static void
on_update (ApMonitor *monitor, const char *name, int strength, ApMonitorTechnology tech)
{
  char *s_icon, *t_icon;

  s_icon = icon_to_string (ap_monitor_get_icon_for_signal (strength));
  t_icon = icon_to_string (ap_monitor_get_icon_for_tech (tech));

  g_print ("AP '%s'\n", name);
  g_print ("\tstrength: %d%% (icon: %s)\n", strength, s_icon);
  g_print ("\ttech: %s (icon: %s)\n", ap_monitor_tech_to_string (tech), t_icon);

  g_free (s_icon);
  g_free (t_icon);
}

int
main (int argc, char **argv)
{
  ApMonitor *apmon;
  GMainLoop *loop;

  /* Setup the locale so that UTF-8 works */
  setlocale (LC_ALL, "");

  loop = g_main_loop_new (NULL, TRUE);

  apmon = ap_monitor_new ();
  g_signal_connect (apmon, "found", G_CALLBACK (on_found), NULL);
  g_signal_connect (apmon, "update", G_CALLBACK (on_update), NULL);
  g_signal_connect (apmon, "lost", G_CALLBACK (on_lost), NULL);

  g_main_loop_run (loop);;

  return 0;
}
