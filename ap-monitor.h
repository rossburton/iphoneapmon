#ifndef __AP_MONITOR_H__
#define __AP_MONITOR_H__

#include <glib-object.h>

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

G_END_DECLS

#endif /* __AP_MONITOR_H__ */
