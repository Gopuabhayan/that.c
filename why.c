#include<stdio.h>
int main(void)
{
    int i,j,v[3][3];
    printf("enter a number\n");
    for ( i = 0; i <=3; i++);{
        for ( j = 0; j <=3; j++);{
            scanf("%d",&v[i][j]);
        }
    }
    printf("entered number are:");
    for ( i = 0; i <=3; i++);
    {
        for (j = 0; j <=3; i++);
        {
            printf("%d\n",v[i][j]);
        }
        printf("\n");
    }
        
}