/*
 * vic20-stubs.c - dummies for unneeded/unused functions
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
 *
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

#include <stddef.h>
#include <stdbool.h>

#include "c64/cart/clockport.h"
#include "cartridge.h"
#include "pet/petpia.h"
#include "vic20.h"

/*******************************************************************************
    clockport
*******************************************************************************/

clockport_supported_devices_t clockport_supported_devices[] = { { 0, NULL } };

/*******************************************************************************
    cartridge
*******************************************************************************/

int cartridge_enable(int crtid)
{
    return -1;
}

int cartridge_disable(int crtid)
{
    return -1;
}

cartridge_info_t *cartridge_get_info_list(void)
{
    return NULL;
}

/* return cartridge type of main slot
   returns 0 (CARTRIDGE_CRT) if crt file */
int cartridge_get_id(int slot)
{
    return CARTRIDGE_NONE;
}

/* FIXME: terrible name, we already have cartridge_get_file_name */
char *cartridge_get_filename(int slot)
{
    return NULL;
}

void cartridge_trigger_freeze(void)
{
}

bool pia1_get_diagnostic_pin(void)
{
    return false;
}
