#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <pigpio.h>
 
#define GPIO_TRIGGER  17
#define GPIO_ECHO  27
#define HIGH        1
#define LOW        0
 
uint32_t start, dist;
 
void my_ctrl_c_handler(int sig){ 
    gpioTerminate();
    exit(0);
}
 void myInterrupt (int gpio, int level, uint32_t tick)
{
    if(HIGH == level){
        start = tick;
    }
    else if(LOW == level){
        dist = tick - start;
    }
}
 
int main( void )
{
    unsigned long ret;
    int loop = 0, i;
    float distance;
    printf( "Raspberry Pi pigpio HC-SR04 UltraSonic sensor program\n" );
 
    setpriority(PRIO_PROCESS, 0, -20);    
    gpioCfgClock(2, 1, 1);
    if (gpioInitialise()<0) return 1;
    signal(SIGINT, my_ctrl_c_handler);      
    gpioSetMode(GPIO_TRIGGER, PI_OUTPUT);
    gpioSetMode(GPIO_ECHO, PI_INPUT);
  
    i = gpioSetAlertFunc(GPIO_ECHO, myInterrupt);
    gpioWrite(GPIO_TRIGGER, LOW);
    gpioDelay(1000 * 1000);
 
    while ( 1 )
    {
        start = dist = 0;
        gpioTrigger(GPIO_TRIGGER, 10, HIGH);    
        gpioSleep(PI_TIME_RELATIVE,  0,  500000);        if(dist && start){
            distance = (float)(dist) / 58.8235;
            printf( "interval[%d]us, Distance : %9.1f cm\n", dist, distance );
        }
        else{
            printf( "sense error : \n");
        }
        gpioDelay(1000 * 1000);
 
    }
    gpioTerminate();
    return(0);
}
 
