#include<stdio.h>
int main()
{
    int  year;
    printf("enter a number\n");
    scanf("%d",&year);
    if (year % 400 == 0)
    {
        printf("this is a leap year\n");
    }
    else if (year %100 == 0)
    {
        printf("this is a leap year\n");
    }
    else if (year % 4 ==0)
    {
        printf("this is a leap year\n");
    }
    else{
        printf("this is not a leap year\n");
    } 
}