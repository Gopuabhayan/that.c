#include<stdio.h>
int main()
{
    int  year;
    printf("enter a number\n");
    scanf("%d",&year);
    if (year % 400)
    {
        printf("this is a leap year\n");
    }
    else if (year %100)
    {
        printf("this is a leap year");
    }
    else if (year % 4)
    {
        printf("this is a leap year");
    }
    else{
        printf("this is not a leap year");
    } 
}