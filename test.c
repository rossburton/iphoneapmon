#include "ap-monitor.h"

int
main (int argc, char **argv)
{
  ApMonitor *apmon;
  GMainLoop *loop;

  g_type_init ();

  loop = g_main_loop_new (NULL, TRUE);

  apmon = ap_monitor_new ();

  g_main_loop_run (loop);;

  return 0;
}
