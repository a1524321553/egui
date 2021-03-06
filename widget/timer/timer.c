/**
 * @file timer.c
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

/* For the use of struct itimerspec */
# define _POSIX_C_SOURCE 199309L

# include "timer.h"

# include <stdlib.h>
# include <unistd.h>
# include <time.h>
# include <signal.h>
# include <sys/types.h>

# include <log.h>

struct timer_internal_data_t {
    timer_t timer;
    struct itimerspec its;
};

static void do_handle_event(union sigval subscribe_info);

struct timer* timer_init(si_t id)
{
    struct timer * addr;

    /* 分配存储空间 */
    addr = calloc(1, sizeof(struct timer));

    if(addr == NULL) {
        EGUI_PRINT_SYS_ERROR("fail to malloc");
        return NULL;
    }

    addr->parent = NULL;
    addr->lchild = NULL;
    addr->rchild = NULL;
    addr->name = "struct timer";
    addr->id = id;
    addr->input_enable = 0;
    addr->update_enable = 0;
    addr->visible = 0;
    addr->keybd_focus = 0;
    addr->is_window = 0;
    addr->callback = NULL;

    addr->internal_data = calloc(1, sizeof(struct timer_internal_data_t));

    /* struct timer 的成员 */

    list_init(&addr->subscribe_info_list);

    {
        union sigval sigval = {
            .sival_ptr = &addr->subscribe_info_list
        };
        static struct sigevent event = {
            .sigev_notify = SIGEV_THREAD,
            .sigev_notify_function = do_handle_event,
            .sigev_notify_attributes = NULL,
        };
        event.sigev_value = sigval;
        timer_create(CLOCK_MONOTONIC, &event, &addr->internal_data->timer);
    }

    return addr;
}

/*
    一般用户并不需要调用这个
    用户应用程序退出之后，application 会释放这个控件所占的空间。
    而窗口管理程序会释放所有资源，包括申请的每个图形设备。
*/
si_t timer_exit(struct timer * b)
{
    list_exit(&b->subscribe_info_list);
    free(b->internal_data);
    free(b);
    return 0;
}

void timer_set_time(struct timer *timer, const struct timespec *value)
{
    timer->internal_data->its.it_value = *value;
}

void timer_run(struct timer *timer)
{
    timer_settime(timer->internal_data->timer, 0,
            &timer->internal_data->its, NULL);
}

/* ---------------------------------------------------------- */
/*                         events                             */
/* ---------------------------------------------------------- */
static void do_handle_event(union sigval subscribe_info)
{
    struct list *subscribe_info_list = subscribe_info.sival_ptr;
    struct list_node *pos;
    list_for_each_macro(subscribe_info_list, pos) {
        struct widget_subscribe_info *si =
            pos->data;
        si->handler(si->subscriber, si->publisher, TIMER_EVENT_ALL);
    }
}

void timer_register_event_handler(struct timer *pub, struct widget *sub, si_t event, widget_event_handler handler)
{
    struct widget_subscribe_info si;
    si.subscriber = sub;
    si.publisher = WIDGET_POINTER(pub);
    si.event = event;
    si.handler = handler;
    list_push_back(&pub->subscribe_info_list, &si, sizeof(si));
}


si_t timer_default_callback(void* t, void* m)
{
    widget_default_callback(t, m);
    return 0;
}
