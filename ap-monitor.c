#include <string.h>
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-glib/glib-watch.h>

#include "ap-monitor.h"
#include "marshals.h"

struct _ApMonitorPrivate {
  /* Avahi <-> GLib adaptors */
  AvahiGLibPoll *poll;
  /* Avahi client */
  AvahiClient *client;
  /* Service browser */
  AvahiServiceBrowser *browser;
  /* Hash of service name to ServiceResolver instances */
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
on_resolve_callback(AvahiServiceResolver *r,
                    AvahiIfIndex interface, AvahiProtocol protocol,
                    AvahiResolverEvent event,
                    const char *name, const char *type,
                    const char *domain, const char *host_name,
                    const AvahiAddress *address,
                    uint16_t port,
                    AvahiStringList *txt,
                    AvahiLookupResultFlags flags,
                    void* userdata)
{
  ApMonitor *self = AP_MONITOR (userdata);
  ApMonitorPrivate *priv = self->priv;

  switch (event) {
  case AVAHI_RESOLVER_FOUND:
    {
      AvahiStringList *l;
      char *value;
      int strength;
      ApMonitorTechnology tech;

      g_debug ("resolved %s", name);

      l = avahi_string_list_find (txt, "ss");
      avahi_string_list_get_pair (l, NULL, &value, NULL);
      if (value)
        strength = (int)(value[0]) / 5.0 * 100;
      else
        strength = 0;
      avahi_free (value);

      l = avahi_string_list_find (txt, "di");
      avahi_string_list_get_pair (l, NULL, &value, NULL);
      if (g_str_equal (value, "N/A")) {
        tech = ApMonitorTechNone;
        strength = 0;
      } else if (g_str_equal (value, "3G")) {
        tech = ApMonitorTechUMTS;
      } else if (g_str_equal (value, "3_5G")) {
        tech = ApMonitorTechHSDPA;
      } else if (g_str_equal (value, "3_75G")) {
        tech = ApMonitorTechHSPA;
      } else {
        g_message ("Unknown di '%s'", value);
        tech = ApMonitorTechUnknown;
      }
      avahi_free (value);

      g_signal_emit (self, signals[UPDATE], 0, strength, tech);
    }
    break;
  }
}


static void
on_browse_callback (AvahiServiceBrowser *b,
                    AvahiIfIndex interface, AvahiProtocol protocol,
                    AvahiBrowserEvent event,
                    const char *name,
                    const char *type,
                    const char *domain,
                    AvahiLookupResultFlags flags,
                    void* userdata)
{
  ApMonitor *self = AP_MONITOR (userdata);
  ApMonitorPrivate *priv = self->priv;

  switch (event) {
  case AVAHI_BROWSER_NEW:
    g_debug ("found %s", name);

    avahi_service_resolver_new (priv->client,
                                interface, protocol, name, type, domain,
                                AVAHI_PROTO_UNSPEC, 0, on_resolve_callback, self);
    /* TODO: error handling */
    break;
  }
}

static void
on_client_state_changed (AvahiClient *client, AvahiClientState state, void *user_data)
{
  ApMonitor *self = AP_MONITOR (user_data);
  ApMonitorPrivate *priv = self->priv;

  switch (state) {
  case AVAHI_CLIENT_S_RUNNING:
    {
      priv->browser = avahi_service_browser_new (client,
                                                 AVAHI_IF_UNSPEC,
                                                 AVAHI_PROTO_UNSPEC,
                                                 "_link411._udp",
                                                 NULL, 0,
                                                 on_browse_callback, self);
      /* TODO: error handling */
    }
    break;
  case AVAHI_CLIENT_S_REGISTERING:
  case AVAHI_CLIENT_CONNECTING:
    /* Silently do nothing */
    break;
  case AVAHI_CLIENT_S_COLLISION:
  case AVAHI_CLIENT_FAILURE:
    g_message ("Cannot connect to Avahi: state %d", state);
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
  int error;

  priv = self->priv = GET_PRIVATE (self);

  priv->poll = avahi_glib_poll_new (NULL, G_PRIORITY_DEFAULT);
  /* TODO free this in finalize */

  priv->client = avahi_client_new (avahi_glib_poll_get (priv->poll),
                                   AVAHI_CLIENT_NO_FAIL,
                                   on_client_state_changed,
                                   self,
                                   &error);
  /* TODO: error checking, use GInitiable */
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
