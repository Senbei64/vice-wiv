/*! \file cia.h
 *
 *  \brief Definitions for MOS6526 (CIA) chip emulation.
 *
 *  \author Jouko Valta <jopi@stekt.oulu.fi>
 *  \author Andre Fachat <fachat@physik.tu-chemnitz.de>
 *  \author Andreas Boose <viceteam@t-online.de>
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

#ifndef VICE_CIA_H
#define VICE_CIA_H

#include "types.h"

/* MOS 6526 models */
#define CIA_MODEL_6526  0  /* "old" */
#define CIA_MODEL_6526A 1  /* "new" */

/* MOS 6526 registers */
#define CIA_PRA         0  /* Port A */
#define CIA_PRB         1  /* Port B */
#define CIA_DDRA        2  /* Data direction register for port A */
#define CIA_DDRB        3  /* Data direction register for port B */

#define CIA_TAL         4  /* Timer A low */
#define CIA_TAH         5  /* Timer A high */
#define CIA_TBL         6  /* Timer B low */
#define CIA_TBH         7  /* Timer B high */

#define CIA_TOD_TEN     8  /* Time Of Day clock 1/10 s */
#define CIA_TOD_SEC     9  /* Time Of Day clock sec */
#define CIA_TOD_MIN     10 /* Time Of Day clock min */
#define CIA_TOD_HR      11 /* Time Of Day clock hour */

#define CIA_SDR         12 /* Serial port shift register */
#define CIA_ICR         13 /* Interrupt control register */
#define CIA_CRA         14 /* Control register A */
#define CIA_CRB         15 /* Control register B */

/* Common control register bits */
#define CIA_CR_START            0x01
#define CIA_CR_PBON             0x02
#define CIA_CR_OUTMODE          0x04
#  define CIA_CR_OUTMODE_TOGGLE         0x04
#  define CIA_CR_OUTMODE_PULSE          0x00
#define CIA_CR_RUNMODE          0x08
#  define CIA_CR_RUNMODE_ONE_SHOT       0x08
#  define CIA_CR_RUNMODE_CONTINUOUS     0x00
#define CIA_CR_LOAD             0x10
#define CIA_CR_FORCE_LOAD               0x10

/* CR A control register bits */
#define CIA_CRA_INMODE          0x20
#  define CIA_CRA_INMODE_CNT            0x20
#  define CIA_CRA_INMODE_PHI2           0x00
#define CIA_CRA_SPMODE          0x40
#  define CIA_CRA_SPMODE_OUT            0x40
#  define CIA_CRA_SPMODE_IN             0x00
#define CIA_CRA_TODIN           0x80
#  define CIA_CRA_TODIN_50HZ            0x80
#  define CIA_CRA_TODIN_60HZ            0x00

/* CR B control register bits */
#define CIA_CRB_INMODE          0x60
#  define CIA_CRB_INMODE_PHI2           0x00
#  define CIA_CRB_INMODE_CNT            0x20
#  define CIA_CRB_INMODE_TA             0x40
#  define CIA_CRB_INMODE_TA_CNT         0x60
#define CIA_CRB_ALARM           0x80
#  define CIA_CRB_ALARM_ALARM           0x80
#  define CIA_CRB_ALARM_TOD             0x00


struct alarm_context_s;
struct cia_context_s;
struct ciat_s;
struct interrupt_cpu_status_s;
struct snapshot_s;

/* Interrupt Masks */
#define CIA_IM_SET      0x80    /* Control Bit */
#define CIA_IM_TA       1       /* Timer A underflow */
#define CIA_IM_TB       2       /* Timer B underflow */
#define CIA_IM_TOD      4       /* TOD Clock Alarm */
#define CIA_IM_SDR      8       /* Shift Register completion */
#define CIA_IM_FLG      16      /* Handshake */
#define CIA_IM_TBB      0x100   /* Timer B bug flag */

typedef struct cia_context_s {
    uint8_t c_cia[16];
    struct alarm_s *ta_alarm;
    struct alarm_s *tb_alarm;
    struct alarm_s *tod_alarm;
    struct alarm_s *idle_alarm;
    struct alarm_s *sdr_alarm;
    unsigned int irqflags;
    uint8_t irq_enabled;
    CLOCK rdi;
    unsigned int tat;
    unsigned int tbt;
    CLOCK todclk;
    unsigned int sr_bits;
    bool sdr_force_finish;
    bool sdr_valid;
    uint16_t shifter;
    uint32_t sdr_delay;           /* delayed actions re. shift register */
    uint8_t old_pa;
    uint8_t old_pb;

    char todstopped;
    char todlatched;
    uint8_t todalarm[4];
    uint8_t todlatch[4];
    CLOCK todticks;               /* init to 100000 */
    uint8_t todtickcounter;

    int power_freq;
    int power_tickcounter;
    CLOCK power_ticks;
    CLOCK ticks_per_sec;

    signed int log;               /* init to LOG_ERR */

    struct ciat_s *ta;
    struct ciat_s *tb;
    CLOCK read_clk;               /* init to 0 */
    int read_offset;              /* init to 0 */
    uint8_t last_read;            /* init to 0 */
    int debugFlag;                /* init to 0 */

    int irq_line;                 /* IK_IRQ or IK_NMI */
    unsigned int int_num;

    char *myname;

    CLOCK *clk_ptr;
    int *rmw_flag;
    int write_offset;             /* 1 if CPU core does CLK++ before store */
    int model;                    /* CIA_MODEL_6526 (old) or ..A (new) */

    bool enabled;
    bool sp_in_state;             /* state stored by ciacore_set_sp() */
    bool cnt_in_state;            /* state stored by ciacore_set_cnt() */
    bool cnt_out_state;           /* state set by shift register output */

    void *prv;                    /* drivecia15{7,8}1_context_t */
    void *context;                /* diskunit_context_t * in 15{7,8}1 */

    void (*undump_ciapa)(struct cia_context_s *, CLOCK, uint8_t);
    void (*undump_ciapb)(struct cia_context_s *, CLOCK, uint8_t);
    void (*store_ciapa)(struct cia_context_s *, CLOCK, uint8_t);
    void (*store_ciapb)(struct cia_context_s *, CLOCK, uint8_t);
    void (*store_sdr)(struct cia_context_s *, uint8_t);
    void (*set_sp)(struct cia_context_s *, CLOCK, bool);
    void (*set_cnt)(struct cia_context_s *, CLOCK, bool);
    uint8_t (*read_ciapa)(struct cia_context_s *);
    uint8_t (*read_ciapb)(struct cia_context_s *);
    void (*read_ciaicr)(struct cia_context_s *);
    void (*read_sdr)(struct cia_context_s *);
    void (*cia_set_int_clk)(struct cia_context_s *, int, CLOCK);
    void (*cia_restore_int)(struct cia_context_s *, int);
    void (*do_reset_cia)(struct cia_context_s *);
    void (*pulse_ciapc)(struct cia_context_s *, CLOCK);
    void (*pre_store)(void);
    void (*pre_read)(void);
    void (*pre_peek)(void);
} cia_context_t;

extern void ciacore_setup_context(struct cia_context_s *cia_context);
extern void ciacore_init(struct cia_context_s *cia_context,
                         struct alarm_context_s *alarm_context,
                         struct interrupt_cpu_status_s *int_status);
extern void ciacore_shutdown(cia_context_t *cia_context);
extern void ciacore_reset(struct cia_context_s *cia_context);
extern void ciacore_disable(struct cia_context_s *cia_context);
extern void ciacore_store(struct cia_context_s *cia_context, uint16_t addr, uint8_t data);
extern uint8_t ciacore_read(struct cia_context_s *cia_context, uint16_t addr);
extern uint8_t ciacore_peek(struct cia_context_s *cia_context, uint16_t addr);

/*
 * The next several functions can be called from outside the CIA
 * to set the FLAG, CNT or SP input lines, or the whole SDR at once
 * (which is cheating).
 */
extern void ciacore_set_flag(struct cia_context_s *cia_context);
extern void ciacore_set_sdr(struct cia_context_s *cia_context, uint8_t data);
extern void ciacore_set_cnt(struct cia_context_s *cia_context, bool data);
extern void ciacore_set_sp(struct cia_context_s *cia_context, bool data);

extern int ciacore_snapshot_write_module(struct cia_context_s *cia_context,
                                         struct snapshot_s *s);
extern int ciacore_snapshot_read_module(struct cia_context_s *cia_context,
                                        struct snapshot_s *s);
extern int ciacore_dump(cia_context_t *cia_context);

#endif
