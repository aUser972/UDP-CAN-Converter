#include <stdio.h>
#include <stdlib.h>
#include "converter.h"

int main()
{
    int first = ConverterUDP2CAN(5000, 0x1, "vcan0", 5, 1024);
    int second = ConverterUDP2CAN(5001, 0x2, "vcan0", 5, 1024);
    int third = ConverterUDP2CAN(5002, 0x3, "vcan1", 5, 1024);
    int fourth = ConverterUDP2CAN(5003, 0x4, "vcan1", 5, 1024);
    ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan0", 5);
    ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan1", 5);
    printf("first: %d\n", first);
    printf("second: %d\n", second);
    for(int i=0; i<10;i++)
    {
        sleep(1);
    }
    StopUDP2CANConvertion(first);
    StopUDP2CANConvertion(second);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
