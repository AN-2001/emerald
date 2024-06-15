#include "utills.h"

int EmrldDateToIndex(char *Date)
{
    int Day, Month;

    Day = (Date[0] - '0') * 10 + (Date[1] - '0');
    Month = (Date[3] - '0') * 10 + (Date[4] - '0');
    return (Month - 1) * 31 + Day;
}
