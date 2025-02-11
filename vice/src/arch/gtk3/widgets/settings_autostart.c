/** \file   settings_autostart.c
 * \brief   GTK3 autostart settings central widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES AutostartDelay                      -vsid
 * $VICERES AutostartDelayRandom                -visd
 * $VICERES AutostartPrgMode                    -vsid
 * $VICERES AutostartPrgDiskImage               -vsid
 * $VICERES AutostartRunWithColon               -vsid
 * $VICERES AutostartBasicLoad                  -vsid
 * $VICERES AutostartTapeBasicLoad              -vsid
 * $VICERES AutostartWarp                       -vsid
 * $VICERES AutostartHandleTrueDriveEmulation   -vsid
 * $VICERES AutostartOnDoubleClick              -vsid
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "machine.h"
#include "resources.h"
#include "autostart-prg.h"
#include "uisettings.h"

#include "settings_autostart.h"


/** \brief  Autostart modes for PRG files
 */
static const vice_gtk3_radiogroup_entry_t autostart_modes[] = {
    { "Virtual FS",         AUTOSTART_PRG_MODE_VFS      /* 0 */ },
    { "Inject into RAM",    AUTOSTART_PRG_MODE_INJECT   /* 1 */ },
    { "Copy to D64",        AUTOSTART_PRG_MODE_DISK     /* 2 */ },
    { NULL,                 -1 }
};


/*
 * Widget helpers
 */

/** \brief  Create widget to control "AutostartDelay" resource
 *
 * \return  grid
 */
static GtkWidget *create_fixed_delay_widget(void)
{
    GtkWidget *layout;
    GtkWidget *label;
    GtkWidget *spin;
    GtkWidget *help;

    layout = vice_gtk3_grid_new_spaced(16, 0);

    label  = gtk_label_new("Autostart fixed delay:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    spin = vice_gtk3_resource_spin_int_new("AutostartDelay", 0, 1000, 1);

    gtk_grid_attach(GTK_GRID(layout), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), spin, 1, 0, 1, 1);

    label = gtk_label_new("seconds");
    gtk_grid_attach(GTK_GRID(layout), label, 2, 0, 1, 1);

    help = gtk_label_new("0 = machine-specific delay for KERNAL boot");
    gtk_grid_attach(GTK_GRID(layout), help, 0, 2, 3, 1);

    gtk_widget_show_all(layout);
    return layout;
}

/** \brief  Create widget to control "AutostartDelay[Random]" resources
 *
 * \return  grid
 */
static GtkWidget *create_delay_widget(void)
{
    GtkWidget *grid;
    GtkWidget *rnd_delay;
    GtkWidget *fix_delay;

    grid = vice_gtk3_grid_new_spaced_with_label(16, 0, "Delay settings", 3);
    gtk_widget_set_margin_top(grid, 8);
    gtk_widget_set_margin_bottom(gtk_grid_get_child_at(GTK_GRID(grid), 0, 0), 8);

    rnd_delay = vice_gtk3_resource_check_button_new("AutostartDelayRandom",
                                                    "Add random delay");
    gtk_widget_set_margin_start(rnd_delay, 16);
    gtk_grid_attach(GTK_GRID(grid), rnd_delay, 0, 2, 1, 1);

    fix_delay = create_fixed_delay_widget();
    gtk_widget_set_margin_start(fix_delay, 16);
    gtk_grid_attach(GTK_GRID(grid), fix_delay, 0, 3, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to control "AutostartPrgDiskImage"
 *
 * \return grid
 */
static GtkWidget *create_prg_diskimage_widget(void)
{
    GtkWidget *grid;
    GtkWidget *image;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "Autostart disk image", 1);

    image = vice_gtk3_resource_browser_new("AutostartPrgDiskImage",
                                           file_chooser_pattern_floppy,
                                           "Disk images",
                                           "Select disk image",
                                           "Path:",
                                           NULL);
    gtk_widget_set_margin_start(image, 16);
    gtk_grid_attach(GTK_GRID(grid), image, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to control PRG-related autostart resources
 *
 * \return  grid
 */
static GtkWidget *create_prg_widget(void)
{
    GtkWidget *grid;
    GtkWidget *colon;
    GtkWidget *basic;
    GtkWidget *tapebasic;
    GtkWidget *mode;
    GtkWidget *group;
    GtkWidget *image;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "PRG settings", 3);
    gtk_widget_set_margin_top(grid, 8);
    gtk_widget_set_margin_bottom(gtk_grid_get_child_at(GTK_GRID(grid), 0, 0), 8);

    colon = vice_gtk3_resource_check_button_new("AutostartRunWithColon",
                                                "Use ':' with RUN");
    gtk_widget_set_margin_start(colon, 16);
    gtk_grid_attach(GTK_GRID(grid), colon, 0, 1, 1, 1);

    tapebasic = vice_gtk3_resource_check_button_new("AutostartTapeBasicLoad",
                                                    "Load to BASIC start (tape)");
    gtk_widget_set_margin_start(tapebasic, 16);
    gtk_grid_attach(GTK_GRID(grid), tapebasic, 0, 2, 1, 1);

    basic = vice_gtk3_resource_check_button_new("AutostartBasicLoad",
                                                "Load to BASIC start (disk)");
    gtk_widget_set_margin_start(basic, 16);
    gtk_grid_attach(GTK_GRID(grid), basic, 0, 3, 1, 1);

    mode = vice_gtk3_grid_new_spaced_with_label(8, 0, "Autostart PRG mode", 1);
    gtk_widget_set_margin_top(mode, 8);
    gtk_widget_set_margin_bottom(gtk_grid_get_child_at(GTK_GRID(mode), 0, 0), 8);
    group = vice_gtk3_resource_radiogroup_new("AutostartPrgMode",
                                              autostart_modes,
                                              GTK_ORIENTATION_VERTICAL);
    gtk_grid_set_row_spacing(GTK_GRID(group), 0);
    gtk_widget_set_margin_start(group, 16);
    gtk_grid_attach(GTK_GRID(mode), group, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mode, 0, 4, 1, 1);

    image = create_prg_diskimage_widget();
    gtk_widget_set_margin_top(image, 8);
    gtk_grid_attach(GTK_GRID(grid), image, 0, 5, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to use in the settings dialog for autostart resources
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  grid
 */
GtkWidget *settings_autostart_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *tde;
    GtkWidget *warp;
    GtkWidget *doubleclick;
    GtkWidget *delay;
    GtkWidget *prg;

    grid = vice_gtk3_grid_new_spaced(16, 0);
    gtk_widget_set_margin_start(grid, 8);
    gtk_widget_set_margin_end(grid, 8);

    tde = vice_gtk3_resource_check_button_new("AutostartHandleTrueDriveEmulation",
                                              "Handle True Drive Emulation on autostart");
    gtk_grid_attach(GTK_GRID(grid), tde, 0, 0, 1, 1);

    warp = vice_gtk3_resource_check_button_new("AutostartWarp",
                                               "Warp on autostart");
    gtk_grid_attach(GTK_GRID(grid), warp, 0, 1, 1, 1);

    doubleclick = vice_gtk3_resource_check_button_new("AutostartOnDoubleClick",
                                                      "Double click for autostart");
    gtk_grid_attach(GTK_GRID(grid), doubleclick, 0, 2, 1, 1);

    delay = create_delay_widget();
    gtk_widget_set_margin_top(delay, 8);
    gtk_grid_attach(GTK_GRID(grid), delay, 0, 3, 1, 1);

    prg = create_prg_widget();
    gtk_widget_set_margin_top(prg, 8);
    gtk_grid_attach(GTK_GRID(grid), prg, 0, 4, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
