#ifndef __TIMER_H__
#define __TIMER_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot/list.h>


/*
 * timer_list struct.
 */
struct timer_list {
	struct list_head list;
	u32_t expires;
	u32_t data;
	void (*function)(u32_t);
};


void init_timer(struct timer_list * timer);
bool_t timer_pending(const struct timer_list * timer);
void add_timer(struct timer_list *timer);
bool_t mod_timer(struct timer_list *timer, u32_t expires);
bool_t del_timer(struct timer_list * timer);
void setup_timer(struct timer_list * timer,	void (*function)(u32_t), u32_t data);

#endif /* __TIMER_LIST_H__ */
