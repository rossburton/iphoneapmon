#include <string.h>
#include <avahi-gobject/ga-client.h>
#include <avahi-gobject/ga-service-browser.h>
#include <avahi-gobject/ga-service-resolver.h>

#include "ap-monitor.h"
#include "marshals.h"

struct _ApMonitorPrivate {
  /* Avahi client */
  GaClient *client;
  /* Service browser */
  GaServiceBrowser *browser;
  /* Hash of service name to GaServiceResolver instances */
  GHashTable *resolvers;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), AP_TYPE_MONITOR, ApMonitorPrivate))

G_DEFINE_TYPE (ApMonitor, ap_monitor, G_TYPE_OBJECT);

enum {
  UPDATE,
  NUM_SIGS
};

static guint signals[NUM_SIGS] = {0,};

static void
on_found (GaServiceResolver *resolver,
          AvahiIfIndex interface, AvahiProtocol protocol,
          const char *name, const char *type, const char *domain,
          const char *host_name, const AvahiAddress * a,
          uint16_t port, GHashTable *txt, AvahiLookupResultFlags flags,
          gpointer user_data)
{
  ApMonitor *self = AP_MONITOR (user_data);
  const char *ss, *di;
  int strength;
  ApMonitorTechnology tech;

  g_debug ("resolved %s", name);

  ss = g_hash_table_lookup (txt, "ss");
  strength = (int)(*ss) / 5.0 * 100;

  di = g_hash_table_lookup (txt, "di");
  if (g_str_equal (di, "N/A")) {
    tech = ApMonitorTechNone;
    strength = 0;
  } else if (g_str_equal (di, "3G")) {
    tech = ApMonitorTechUMTS;
  } else if (g_str_equal (di, "3_5G")) {
    tech = ApMonitorTechHSDPA;
  } else if (g_str_equal (di, "3_75G")) {
    tech = ApMonitorTechHSPA;
  } else {
    g_message ("Unknown di '%s'", di);
    tech = ApMonitorTechUnknown;
  }

  g_signal_emit (self, signals[UPDATE], 0, strength, tech);
}

static void
on_new_service (GaServiceBrowser *browser,
                AvahiIfIndex interface, AvahiProtocol protocol,
                const char *name, const char *type, const char *domain,
                AvahiLookupResultFlags flags, gpointer user_data)
{
  ApMonitor *self = AP_MONITOR (user_data);
  GaServiceResolver *resolver;
  GError *error = NULL;

  g_debug ("found %s", name);

  resolver = ga_service_resolver_new (interface, protocol,
                                      name, type, domain,
                                      /* TODO: reuse or use ANY? */ protocol,
                                      GA_LOOKUP_NO_FLAGS);

  g_signal_connect (resolver, "found", G_CALLBACK (on_found), self);

  if (!ga_service_resolver_attach (resolver, self->priv->client, &error)) {
    g_message ("Cannot start resolver: %s", error->message);
    g_error_free (error);
  }

  /* TODO store in the hash */
}

static void
on_client_state_changed (GaClient *client, GaClientState state, gpointer user_data)
{
  ApMonitor *self = AP_MONITOR (user_data);

  switch (state) {
  case GA_CLIENT_STATE_S_RUNNING:
    {
      self->priv->browser = ga_service_browser_new ("_link411._udp");

      g_signal_connect (self->priv->browser, "new-service", G_CALLBACK (on_new_service), self);

      ga_service_browser_attach (self->priv->browser, self->priv->client, /* TODO error */NULL);
    }
    break;
  case GA_CLIENT_STATE_S_REGISTERING:
  case GA_CLIENT_STATE_NOT_STARTED:
  case GA_CLIENT_STATE_CONNECTING:
    /* Silently do nothing */
  case GA_CLIENT_STATE_S_COLLISION:
  case GA_CLIENT_STATE_FAILURE:
    g_message ("Cannot connect to Avahi: state %d", state);
    break;
    break;
  }
}

static void
ap_monitor_class_init (ApMonitorClass *klass)
{
    GObjectClass *o_class = (GObjectClass *)klass;

    g_type_class_add_private (klass, sizeof (ApMonitorPrivate));

    signals[UPDATE] = g_signal_new ("update",
                                    AP_TYPE_MONITOR,
                                    G_SIGNAL_RUN_FIRST,
                                    0, NULL, NULL,
                                    ap_marshal_VOID__INT_INT,
                                    G_TYPE_NONE,
                                    2, G_TYPE_INT, G_TYPE_INT);
}

static void
ap_monitor_init (ApMonitor *self)
{
  ApMonitorPrivate *priv;
  GError *error = NULL;

  priv = self->priv = GET_PRIVATE (self);

  priv->client = ga_client_new (GA_CLIENT_FLAG_NO_FAIL);

  g_signal_connect (priv->client, "state-changed",
                    G_CALLBACK (on_client_state_changed), self);

  /* TODO Do this in initable for error handling */
  if (!ga_client_start(priv->client, &error)) {
    g_message ("Cannot start Avahi: %s", error->message);
    g_error_free (error);
  }
}

ApMonitor *
ap_monitor_new (void)
{
  return g_object_new (AP_TYPE_MONITOR, NULL);
}

const char *
ap_monitor_tech_to_string (ApMonitorTechnology tech)
{
  switch (tech) {
  case ApMonitorTechUnknown:
    return "Unknown";
  case ApMonitorTechNone:
    return "None";
  case ApMonitorTechUMTS:
    return "UMTS (3G)";
  case ApMonitorTechHSDPA:
    return "HSDPA (3.5G)";
  case ApMonitorTechHSPA:
    return "HSPA (3.75G)";
  }

  g_warning ("Unhandled technology %d", tech);
  return "unknown";
}
