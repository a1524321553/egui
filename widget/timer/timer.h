/**
 * @file timer.h
 *
 * @author Dalian University of Technology
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 Dalian University of Technology
 *
 * This file is part of EDUGUI.
 *
 * EDUGUI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * EDUGUI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EDUGUI; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * All rights reserved.
**/

# ifndef _TIMER_H_
# define _TIMER_H_ 1

# include "../widget/widget.h"

# include <time.h>

struct timer_internal_data_t;

struct timer {
    WIDGET_DEFINITION
    struct list subscribe_info_list;
    struct timer_internal_data_t *internal_data;
};

enum timer_event_type {
    TIMER_EVENT_ALL
};

# define TIMER_POINTER(addr) ((struct timer *)(addr))

struct timer* timer_init(si_t id);

si_t timer_exit(struct timer * b);

void timer_register_event_handler(struct timer *pub, struct widget *sub, si_t event, widget_event_handler handler);

void timer_set_time(struct timer *timer, const struct timespec *value);

void timer_run(struct timer *timer);


extern si_t timer_default_callback(void* t, void* m);

# endif
