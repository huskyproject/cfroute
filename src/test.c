#include <dos.h>
#include <time.h>
#include <stdio.h>

int main(void)
{
        struct date d;
	struct time t;
	
        getdate (&d);
	gettime (&t);
        printf ("%d %d %d\n", t.ti_hour, t.ti_min, t.ti_sec);
        printf ("%d %d %d\n", d.da_day, d.da_mon, d.da_year);

        return 0;
}

