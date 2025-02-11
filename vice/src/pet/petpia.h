/*
 * pia.h -- PIA chip emulation.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#ifndef VICE_PIA_H
#define VICE_PIA_H

#include "types.h"
#include <stdbool.h>
#include <stdint.h>

/* Signal values (for signaling edges on the control lines) */

#define PIA_SIG_CA1     0
#define PIA_SIG_CA2     1
#define PIA_SIG_CB1     2
#define PIA_SIG_CB2     3

#define PIA_SIG_FALL    0
#define PIA_SIG_RISE    1

/* ------------------------------------------------------------------------- */

struct snapshot_s;

extern int pia1_resources_init(void);
extern int pia1_cmdline_options_init(void);

extern void pia1_init(void);
extern void pia1_reset(void);
extern void pia1_signal(int line, int edge);
extern void pia1_store(uint16_t addr, uint8_t value);
extern uint8_t pia1_read(uint16_t addr);
extern uint8_t pia1_peek(uint16_t addr);
extern void pia1_set_tape1_sense(int v);
extern void pia1_set_tape2_sense(int v);
extern void pia1_set_tape1_write_in(int v);
extern void pia1_set_tape2_write_in(int v);
extern void pia1_set_tape1_motor_in(int v);
extern void pia1_set_tape2_motor_in(int v);
extern bool pia1_get_diagnostic_pin(void);
extern int pia1_snapshot_read_module(struct snapshot_s *);
extern int pia1_snapshot_write_module(struct snapshot_s *);

extern void pia2_init(void);
extern void pia2_reset(void);
extern void pia2_signal(int line, int edge);
extern void pia2_store(uint16_t addr, uint8_t value);
extern uint8_t pia2_read(uint16_t addr);
extern uint8_t pia2_peek(uint16_t addr);

extern int pia1_dump(void);
extern int pia2_dump(void);

extern int pia2_snapshot_read_module(struct snapshot_s *);
extern int pia2_snapshot_write_module(struct snapshot_s *);


#endif
