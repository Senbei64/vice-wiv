/*
 * userport_io_sim.h:
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_USERPORT_IO_SIM_H
#define VICE_USERPORT_IO_SIM_H

#include "types.h"

extern int userport_io_sim_resources_init(void);

extern void userport_io_sim_set_pbx_ddr_lines(uint8_t val);
extern uint8_t userport_io_sim_get_pbx_ddr_lines(void);

extern void userport_io_sim_set_pbx_out_lines(uint8_t val);
extern uint8_t userport_io_sim_get_pbx_out_lines(void);
extern uint8_t userport_io_sim_get_raw_pbx_out_lines(void);

extern uint8_t userport_io_sim_get_pbx_in_lines(void);
extern uint8_t userport_io_sim_get_raw_pbx_in_lines(void);

#endif
