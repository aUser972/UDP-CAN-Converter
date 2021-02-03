#include <stdio.h>
#include <stdlib.h>
#include "converter.h"

int main()
{
    char ch;
    int first = ConverterUDP2CAN(0x1, "vcan0", 5, 1024, 5000);
    int second = ConverterUDP2CAN(0x2, "vcan0", 5, 1024, 5001);
    int third = ConverterUDP2CAN(0x3, "vcan1", 5, 1024, 5002);
    int fourth = ConverterUDP2CAN(0x4, "vcan1", 5, 1024, 5003);
    int fifth = ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan0", 5);
    int sixth = ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan1", 5);


    printf("press 'c' to stop converting");
    while(1)
    {
        ch = getc(stdin);
        if(ch == 'c')
        {
            StopUDP2CANConvertion(first);
            StopUDP2CANConvertion(second);
            StopUDP2CANConvertion(third);
            StopUDP2CANConvertion(fourth);
            StopCAN2UDPConvertion(fifth);
            StopCAN2UDPConvertion(sixth);
            break;
        }
        else
        {
            sleep(1);
        }

    }
    return 0;
}
