// Date & time functions.
// Don't ask me how this works. I can't tell. The magic here
// comes from TurboPower's Object Professional for C++. They
// also took if from somewhere...

#ifdef OS_2
#ifdef EMX
#include <os2emx.h>
#else
#include <os2.h>
#endif
#endif

#include "structs.hpp"

long GetJulianDate(int Day, int Month, int Year)
{
        if (Year < 300)
        {
                DateTime d;
                d.getCurrentTime();

                Year += 1900;

                /* context sensitive interpretation of two digit year number */
                while (Year + 50 < d.year) { Year += 100; }
        }
	if (Year < 1900)
		Year += 100;
	if (Month > 2)
		Month -= 3;
	else
	{
		Month += 9;
		Year--;
	}
	Year -= 1600;
	return (((((long)Year / 100)  *146097L) / 4) +
		  ((((long)Year % 100)  *1461) / 4) +
		  (((153  *(long)Month) + 2) / 5) + Day + 59);
};

long GetToday (void)
{
        DateTime d;
        d.getCurrentTime();
        return GetJulianDate (d.day,d.month,d.year);
}

int GetDOW (void)
{
	return ((int) ((GetToday()+5)%7));
}

