#include "calc.h"

float div(int first_num, int second_num)
{
    if(second_num == 0) return 0;
    else return (float)first_num / second_num;
}