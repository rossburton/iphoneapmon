#include "ap-monitor.h"

static void
on_update (ApMonitor *monitor, int strength, int tech)
{
  g_print ("Strengh %d%% %s\n", strength, ap_monitor_tech_to_string (tech));
}

int
main (int argc, char **argv)
{
  ApMonitor *apmon;
  GMainLoop *loop;

  g_type_init ();

  loop = g_main_loop_new (NULL, TRUE);

  apmon = ap_monitor_new ();
  g_signal_connect (apmon, "update", G_CALLBACK (on_update), NULL);

  g_main_loop_run (loop);;

  return 0;
}
