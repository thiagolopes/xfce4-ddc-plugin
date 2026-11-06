#ifdef HAVE_XFCE_REVISION_H
#include "xfce-revision.h"
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(__linux__) || defined(__GNU__)
#include <sys/vfs.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD_kernel__)
#include <sys/param.h>
#include <sys/mount.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

typedef struct{
    XfcePanelPlugin *plugin;

    GtkWidget *box;
    GtkWidget *ebox;
    GtkWidget *panel_button;
} XfceDDC;

ddc_plugin_about(XfcePanelPlugin *plugin, XfceDDC *ddc) {
    cosnt gchar *about_data[] = {
        "Thiago Lopes <thiagolopes@pm.me>",
        NULL
    };
    gtk_show_about_dialog(NULL,
                          "logo-icon-name", "xfce4-ddc-plugin",
                          "license", xfce_get_license_text (XFCE_LICENSE_TEXT_BSD),
                          "version", VERSION_FULL,
                          "program-name", PACKAGE_NAME,
                          "comments", _("DDC"),
                          "website", PACKAGE_URL,
                          "copyright", "Copyright \302\251 2003-" COPYRIGHT_YEAR " The Xfce development team",
                          "authors", about_data, NULL);
}

static void ddc_construct(XfcePanelPlugin *plugin) {
    XfceDDC *ddc;
    ddc = g_new0(XfceDDC, 1);
    ddc->plugin = plugin;

    GtkWidget *label = gtk_label_new("Teste!!!");
    gtk_widget_show(label);

    gtk_container_add(plugin, label);
}
