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

#ifndef __AP_MONITOR_H__
#define __AP_MONITOR_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef enum {
  ApMonitorTechUnknown = -1,
  ApMonitorTechNone = 0,
  ApMonitorTechUMTS, /* 3G */
  ApMonitorTechHSDPA, /* 3.5G */
  ApMonitorTechHSPA /* 3.75G */
} ApMonitorTechnology;

#define AP_TYPE_MONITOR (ap_monitor_get_type())
#define AP_MONITOR(obj)                                                 \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                AP_TYPE_MONITOR,                        \
                                ApMonitor))
#define AP_MONITOR_CLASS(klass)                                         \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             AP_TYPE_MONITOR,                           \
                             ApMonitorClass))
#define IS_AP_MONITOR(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                AP_TYPE_MONITOR))
#define IS_AP_MONITOR_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             AP_TYPE_MONITOR))
#define AP_MONITOR_GET_CLASS(obj)                                       \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               AP_TYPE_MONITOR,                         \
                               ApMonitorClass))

typedef struct _ApMonitorPrivate ApMonitorPrivate;
typedef struct _ApMonitor      ApMonitor;
typedef struct _ApMonitorClass ApMonitorClass;

struct _ApMonitor {
  GObject parent;

  ApMonitorPrivate *priv;
};

struct _ApMonitorClass {
  GObjectClass parent_class;
};

GType ap_monitor_get_type (void) G_GNUC_CONST;

ApMonitor *ap_monitor_new (void);

const char * ap_monitor_tech_to_string      (ApMonitorTechnology tech);
GIcon *      ap_monitor_get_icon_for_tech   (ApMonitorTechnology tech);
GIcon *      ap_monitor_get_icon_for_signal (int signal);

G_END_DECLS

#endif /* __AP_MONITOR_H__ */
