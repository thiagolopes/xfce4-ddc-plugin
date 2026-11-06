#ifdef HAVE_XFCE_REVISION_H
#include "xfce-revision.h"
#endif

#include <string.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

#include <ddcutil_c_api.h>
#include <ddcutil_status_codes.h>

typedef struct {
    XfcePanelPlugin *plugin;
    bool error;
    char error_msg[256];

    GtkWidget *box;
    GtkWidget *ebox;
} XfceDDC;

static void ddc_construct(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER(ddc_construct);


static void
ddc_plugin_about(XfcePanelPlugin *plugin, XfceDDC *ddc)
{
    const gchar *about_data[] = {
        "Xfce development team <xfce4-dev@xfce.org>",
        "Thiago Lopes <thiagolopes@pm.me>",
        NULL
    };
    gtk_show_about_dialog(NULL,
                          "logo-icon-name", "video-display",
                          "license", xfce_get_license_text (XFCE_LICENSE_TEXT_BSD),
                          "version", VERSION_FULL,
                          "program-name", PACKAGE_NAME,
                          "comments", _("DDC"),
                          "website", PACKAGE_URL,
                          "copyright", "Copyright \302\251 2003-" COPYRIGHT_YEAR " The Xfce development team",
                          "authors", about_data, NULL);
}


static void
ddc_set_percent(GtkWidget *widget, gpointer perc_data)
{
    gint perc = GPOINTER_TO_INT(perc_data);

    DDCA_Display_Handle conn = NULL;
    DDCA_Display_Info_List *dlist = NULL;
    DDCA_Status rc;

    rc = ddca_get_display_info_list2(false, &dlist);
    if ( rc != DDCRC_OK || dlist == NULL || dlist->ct == 0) {
        goto exit;
    }

    DDCA_Display_Ref dref = dlist->info[0].dref;
    rc = ddca_open_display2(dref, false, &conn);
    if (rc != DDCRC_OK) {
        goto exit;
    }

    /* DDCA_Non_Table_Vcp_Value val; */
    /* ddca_get_non_table_vcp_value(conn, 0x10, &val); */
    ddca_set_non_table_vcp_value(conn, 0x10, 0x00, perc);

 exit:
    ddca_free_display_info_list(dlist);
    ddca_close_display(conn);
}


static void
dcc_menu_construct(GtkWidget *widget, gpointer* state_point)
{
    XfceDDC *state = (XfceDDC*)state_point;
    GtkWidget *menu = gtk_menu_new();

    if (state->error) {
        char* error = state->error_msg;
        GtkWidget *it = gtk_menu_item_new_with_label(error);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), it);
    }
    else {
        for(int i = 1; i <= 10; i ++) {
            char buff[32] = {0};
            snprintf(buff, sizeof(buff), "%d%%", i*10);
            GtkWidget *it = gtk_menu_item_new_with_label(buff);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), it);
            g_signal_connect(it, "activate", G_CALLBACK(ddc_set_percent), GINT_TO_POINTER(i * 10));
        }
    }
    gtk_widget_show_all(menu);
    gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(widget),
                             GDK_GRAVITY_SOUTH_WEST,
                             GDK_GRAVITY_NORTH_WEST,
                             NULL);
}


static void
ddc_construct(XfcePanelPlugin *plugin)
{
    XfceDDC *ddc = {0};
    GtkWidget *btn;
    /* GtkWidget *btn_box; */
    GtkWidget *btn_icon;
    /* GtkWidget *btn_label; */

    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    // new
    GtkOrientation orientation = xfce_panel_plugin_get_orientation(plugin);
    ddc = g_new0(XfceDDC, 1);
    ddc->plugin = plugin;
    DDCA_Status s = ddca_init(NULL, DDCA_SYSLOG_NOT_SET, DDCA_INIT_OPTIONS_NONE); // DDC protocol init
    if (s != DDCRC_OK) {
        ddc->error = true;
        const char *detail = "Error on DDC start";
        DDCA_Error_Detail *err = ddca_get_error_detail();
        if (err != NULL) {
            if (err->detail != NULL)
                detail = err->detail;
            ddca_free_error_detail(err);
        }
        strncpy(ddc->error_msg, detail, sizeof(ddc->error_msg) - 1);
        ddc->error_msg[sizeof(ddc->error_msg) - 1] = '\0';
    }

    // ui
    ddc->ebox = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(ddc->ebox), FALSE);
    ddc->box = gtk_box_new(orientation, 0);
    gtk_container_add(GTK_CONTAINER(plugin), ddc->ebox);
    gtk_container_add(GTK_CONTAINER(ddc->ebox), ddc->box);

    btn = xfce_panel_create_button();
    /* ddc->btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); */
    /* gtk_box_set_spacing(GTK_BOX(ddc->btn_box), 0); */
    btn_icon = gtk_image_new_from_icon_name("display-brightness-symbolic", xfce_panel_plugin_get_icon_size(plugin));
    /* ddc->btn_label = gtk_label_new("Monitor"); */
    gtk_container_add(GTK_CONTAINER(ddc->box), btn);
    /* gtk_container_add(GTK_CONTAINER(ddc->btn), ddc->btn_box); */
    /* gtk_box_pack_start(GTK_BOX(ddc->btn_box), ddc->btn_icon, FALSE, FALSE, 0); */
    /* gtk_box_pack_start(GTK_BOX(ddc->btn_box), ddc->btn_label, FALSE, FALSE, 0); */
    gtk_container_add(GTK_CONTAINER(btn), btn_icon);

    // signal
    g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(dcc_menu_construct), ddc);

    // subscribe
    /* xfce_panel_plugin_add_action_widget(plugin, ddc->ebox); */
    /* xfce_panel_plugin_add_action_widget(plugin, btn); */

    // show all
    /* gtk_widget_set_size_request(ddc->ebox, -1, -1); */
    gtk_widget_show_all(ddc->ebox);

    // about
    g_signal_connect(plugin, "about", G_CALLBACK(ddc_plugin_about), ddc);
    xfce_panel_plugin_menu_show_about (plugin);
}
