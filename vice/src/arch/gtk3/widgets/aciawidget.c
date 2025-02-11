/** \file   aciawidget.c
 * \brief   Widget to control various ACIA related resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *
 * $VICERES Acia1Dev    -x64dtv -vsid
 * $VICERES Acia1Base   x64 x64sc xscpu64 xvic x128
 * $VICERES RsDevice1   all
 * $VICERES RsDevice2   all
 */

/*
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "ui.h"
#include "vice_gtk3.h"

#include "aciawidget.h"


/** \brief  Reference to baud rates list
 */
static int *acia_baud_rates;


/** \brief  References to the ACIA GtkEntry widgets
 *
 * I wish there was a better solution.
 */
static GtkWidget *acia_entries[2];


/** \brief  List of baud rates
 *
 * Set in acia_widget_create()
 */
static vice_gtk3_combo_entry_int_t *baud_rate_list = NULL;


/** \brief  List of ACIA devices
 */
static const vice_gtk3_radiogroup_entry_t acia_device_list[] = {
    { "Serial 1",      0 },
    { "Serial 2",      1 },
    { "Dump to file",  2 },
    { "Exec process",  3 },
    { NULL,           -1 }
};


/** \brief  Generate heap-allocated list to use in a resourcecombobox
 *
 * Creates a list of ui_combo_box_int_t entries from the `acia_baud_rates` list
 */
static void generate_baud_rate_list(void)
{
    unsigned int i;

    /* count number of baud rates */
    for (i = 0; acia_baud_rates[i] > 0; i++) {
        /* NOP */
    }

    baud_rate_list = lib_malloc((i + 1) * sizeof *baud_rate_list);
    for (i = 0; acia_baud_rates[i] > 0; i++) {
        baud_rate_list[i].name = lib_msprintf("%d", acia_baud_rates[i]);
        baud_rate_list[i].id = acia_baud_rates[i];
    }
    /* terminate list */
    baud_rate_list[i].name = NULL;
    baud_rate_list[i].id = -1;
}


/** \brief  Free memory used by `baud_rate_list`
 */
static void free_baud_rate_list(void)
{
    int i;

    for (i = 0; baud_rate_list[i].name != NULL; i++) {
        lib_free(baud_rate_list[i].name);
    }
    lib_free(baud_rate_list);
    baud_rate_list = NULL;
}


/** \brief  Handler for the 'destroy' event of the main widget
 *
 * Frees memory used by the baud rate list
 *
 * \param[in]   widget      main widget (unused)
 * \param[in]   user_data   extra event data (unused)
 */
static void on_destroy(GtkWidget *widget, gpointer user_data)
{
    free_baud_rate_list();
}


/** \brief  Handler for the 'changed' event of a serial device text box
 *
 * \param[in]   widget      text box triggering the event
 * \param[in]   user_data   serial device number (`int`)
 */
static void on_serial_device_changed(GtkWidget *widget, gpointer user_data)
{
    int device = GPOINTER_TO_INT(user_data);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));
    resources_set_string_sprintf("RsDevice%d", text, device);
}


/** \brief  Callback for the SuperPET ACIA host serial device path browser
 *
 * \param[in,out]   dialog      open-file dialog
 * \param[in]       filename    path to host serial device
 * \param[in]       data        device number (1 or 2)
 */
static void browse_filename_callback(GtkDialog *dialog,
                                     gchar *filename,
                                     gpointer data)
{
    if (filename != NULL) {
        int device = GPOINTER_TO_INT(data);

        if (device != 1 && device != 2) {
            log_error(LOG_ERR, "%s:%d:%s(): invalid CIA device number: %d",
                    __FILE__, __LINE__, __func__, device);
        } else {
            GtkWidget *entry = acia_entries[device - 1];

            /* update text entry box, forces an update of the resource */
            gtk_entry_set_text(GTK_ENTRY(entry), filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Handler for the 'clicked' event of the 'browse' buttons
 *
 * \param[in]   widget      button triggering the event
 * \param[in]   user_data   device number (`int`)
 */
static void on_browse_clicked(GtkWidget *widget, gpointer device)
{
    const char *flist[] = { "ttyS*", NULL };
    gchar       title[256];
    int         devnum;

    devnum = GPOINTER_TO_INT(device);
    g_snprintf(title, sizeof title, "Select serial device #%d", devnum);

    vice_gtk3_open_file_dialog(title,
                               "Serial ports",
                               flist,
                               "/dev",
                               browse_filename_callback,
                               device);
}

/** \brief  Create an ACIA device widget
 *
 * Creates a widget to select an ACIA device.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_acia_device_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "Acia device", 2);
    gtk_widget_set_margin_bottom(gtk_grid_get_child_at(GTK_GRID(grid), 0, 0), 8);

    radio_group = vice_gtk3_resource_radiogroup_new("Acia1Dev",
                                                    acia_device_list,
                                                    GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_margin_start(radio_group, 8);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create a widget to set an ACIA serial device (path + baud rate)
 *
 * \param[in]   num     serial device number
 *
 * \return  GtkGrid
 */
static GtkWidget *create_acia_serial_device_widget(int num)
{
    GtkWidget  *grid;
    GtkWidget  *entry;
    GtkWidget  *browse;
    GtkWidget  *label;
    GtkWidget  *combo;
    const char *path;
    char        buffer[256];

    g_snprintf(buffer, sizeof buffer, "Serial %d device", num);
    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, buffer, 4);
    g_object_set_data(G_OBJECT(grid), "SerialDevice", GINT_TO_POINTER(num));
    vice_gtk3_grid_set_title_margin(grid, 8);

    /* add "RsDevice" property to widget to allow the event handlers to set
     * the proper resources
     */
    g_object_set_data(G_OBJECT(grid), "RsDevice", GINT_TO_POINTER(num));

    entry = gtk_entry_new();
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_margin_start(entry, 8);
    browse = gtk_button_new_with_label("Browse");
    g_signal_connect(browse,
                     "clicked",
                     G_CALLBACK(on_browse_clicked),
                     GINT_TO_POINTER(num));
    /* lame, I know */
    acia_entries[num - 1] = entry;

    gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 1, 1, 1, 1);

    label = gtk_label_new("Baud");
    gtk_widget_set_margin_start(label, 8);
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    g_snprintf(buffer, sizeof buffer, "RsDevice%dBaud", num);
    combo = vice_gtk3_resource_combo_box_int_new(buffer, baud_rate_list);

    gtk_grid_attach(GTK_GRID(grid), label, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 3, 1, 1, 1);

    /* set resources*/
    resources_get_string_sprintf("RsDevice%d", &path, num);
    if (path != NULL && *path != '\0') {
        gtk_entry_set_text(GTK_ENTRY(entry), path);
    }

    /* connect handlers */
    g_signal_connect(entry,
                     "changed",
                     G_CALLBACK(on_serial_device_changed),
                     GINT_TO_POINTER(num));

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create ACIA settings widget
 *
 * XXX: currently designed for PET, might need updating when used in other UIs
 *
 * \param[in]   baud    list of baud rates (list of `int`'s, terminated by -1)
 *
 * \return  GtkGrid
 */
GtkWidget *acia_widget_create(int *baud)
{
    GtkWidget *grid;
    GtkWidget *device_widget;
    GtkWidget *serial1_widget;
    GtkWidget *serial2_widget;

    acia_baud_rates = baud;
    generate_baud_rate_list();

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "ACIA settings", 3);
    vice_gtk3_grid_set_title_margin(grid, 8);

    device_widget = create_acia_device_widget();
    gtk_widget_set_margin_start(device_widget, 8);
    gtk_grid_attach(GTK_GRID(grid), device_widget, 0, 1, 1, 2);

    serial1_widget = create_acia_serial_device_widget(1);
    gtk_grid_attach(GTK_GRID(grid), serial1_widget, 1, 1, 1, 1);

    serial2_widget = create_acia_serial_device_widget(2);
    gtk_widget_set_margin_top(serial2_widget, 8);
    gtk_grid_attach(GTK_GRID(grid), serial2_widget, 1, 2, 1, 1);

    g_signal_connect_unlocked(grid, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(grid);
    return grid;
}
