#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include "ext.h"
#include "ext_obex.h"

#define FEB         28
#define HOURS       23
#define LEAP_FEB    29
#define LONG_MONTH  31
#define MINUTES     59
#define NULL_TERM   1
#define SHORT_MONTH 30
#define TIME_LEN    8
#define YEAR_OFFSET 1900

typedef struct _valid_date {
        t_object ob;
        long month;
        long day;
        long hour;
        long minute;
        void *m_outlet0;
        void *m_outlet1;
        void *m_outlet2;
        void *m_outlet3;
        void *m_outlet4;
} t_valid_date;

static t_class *s_valid_date_class;

void ext_main(void *);
int get_feb(void);
void set_day(t_valid_date *, long);
void set_hour(t_valid_date *, long);
void set_minute(t_valid_date *, long);
void set_month(t_valid_date *, long);
void valid_date_bang(t_valid_date *);
void *valid_date_new();

void
ext_main(void *r)
{
        t_class *c;
        
        c = class_new("valid_date", (method)valid_date_new, (method)NULL,
            sizeof(t_valid_date), 0L, 0);
        class_addmethod(c, (method)valid_date_bang, "bang", 0);
        class_addmethod(c, (method)set_month, "in1", A_LONG, 0);
        class_addmethod(c, (method)set_day, "in2", A_LONG, 0);
        class_addmethod(c, (method)set_hour, "in3", A_LONG, 0);
        class_addmethod(c, (method)set_minute, "in4", A_LONG, 0);
        
        class_register(CLASS_BOX, c);
        
        s_valid_date_class = c;
}

int
get_feb(void)
{
        struct tm *info;
        time_t this_time;
        unsigned year;
    
        time(&this_time);
        info = localtime(&this_time);
        year = info->tm_year + YEAR_OFFSET;
    
        if (info->tm_mon > 1)
            ++year;
    
        if ((year % 400) == 0)
            return LEAP_FEB;
        else if ((year % 100) == 0)
            return FEB;
        else if ((year % 4) == 0)
            return LEAP_FEB;
        else
            return FEB;
}

void
set_day(t_valid_date *x, long l)
{
        x->day = l;
}

void
set_hour(t_valid_date *x, long l)
{
        x->hour = l;
}

void
set_minute(t_valid_date *x, long l)
{
        x->minute = l;
}

void
set_month(t_valid_date *x, long l)
{
        x->month = l;
}

void
valid_date_bang(t_valid_date *x)
{
        char status[MAXPATHLEN];
        unsigned month_len;
        if (x->month > 12 || x->month < 1) {
                strcpy(status, "Months must be be between 1 and 12!");
                error(status);
                outlet_anything(x->m_outlet0, gensym(status), 0, NIL);
                return;
        }

        switch (x->month) {
        case 1:
                month_len = LONG_MONTH;
                break;
        case 2: 
                month_len = get_feb();
                break;
        case 3:    
                month_len = LONG_MONTH;
                break;
        case 4:
                month_len = SHORT_MONTH;
                break;
        case 5: 
                month_len = LONG_MONTH;
                break;
        case 6:
                month_len = SHORT_MONTH;
                break;
        case 7:
        case 8:
                month_len = LONG_MONTH;
                break;
        case 9:
                month_len = SHORT_MONTH;
                break;
        case 10:
                month_len = LONG_MONTH;
                break;
        case 11:
                month_len = SHORT_MONTH;
                break;
        case 12:
                month_len = LONG_MONTH;
                break;
        default:
                strcpy(status, "Invalid month!");
                error(status);
                outlet_anything(x->m_outlet0, gensym(status), 0, NIL);
                return;
        }

        if (x->day > month_len || x->day < 1) {
                snprintf(status, MAXPATHLEN, 
                    "Day %ld does not exist in month %ld!", x->day, x->month);
                error(status);
                outlet_anything(x->m_outlet0, gensym(status), 0, NIL);
                return;
        }

        if (x->hour > HOURS || x->hour < 1) {
                strcpy(status, "Hour must be between 0 and 23!");
                error(status);
                outlet_anything(x->m_outlet0, gensym(status), 0, NIL);
                return;
        }

        if (x->minute > MINUTES || x->minute < 0) {
                strcpy(status, "Minute must be between 0 and 59!");
                error(status);
                outlet_anything(x->m_outlet0, gensym(status), 0, NIL);
                return;
        }

        snprintf(status, MAXPATHLEN, "Scheduled for %02ld/%02ld %02ld:%02ld",
            x->day, x->month, x->hour, x->minute);
        post(status);
        outlet_anything(x->m_outlet0, gensym(status), 0, NIL);
        outlet_int(x->m_outlet1, x->month);
        outlet_int(x->m_outlet2, x->day);
        outlet_int(x->m_outlet3, x->hour);
        outlet_int(x->m_outlet4, x->minute);

}

void *
valid_date_new()
{
        t_valid_date *x = (t_valid_date *)object_alloc(s_valid_date_class);
        intin(x, 4);
        intin(x, 3);
        intin(x, 2);
        intin(x, 1);
    
        x->m_outlet4 = intout((t_object *)x);
        x->m_outlet3 = intout((t_object *)x);
        x->m_outlet2 = intout((t_object *)x);
        x->m_outlet1 = intout((t_object *)x);
        x->m_outlet0 = outlet_new((t_object *)x, NULL);
    
        x->month = 0;
        x->day = 0;
        x->hour = 0;
        x->minute = 0;
    
        return (void *)x;
}
