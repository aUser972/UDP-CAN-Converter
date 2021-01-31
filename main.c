#include <stdio.h>
#include <stdlib.h>
#include "converter.h"

int main()
{
    int first = ConverterUDP2CAN(0x1, "vcan0", 5, 1024, 5000);
    int second = ConverterUDP2CAN(0x2, "vcan0", 5, 1024, 5001);
    int third = ConverterUDP2CAN(0x3, "vcan1", 5, 1024, 5002);
    int fourth = ConverterUDP2CAN(0x4, "vcan1", 5, 1024, 5003);
    int fiveth = ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan0", 5);
    int sixth = ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan1", 5);
    for(int i=0; i<3;i++)
    {
        sleep(1);
    }
    StopUDP2CANConvertion(first);
    StopUDP2CANConvertion(second);
    StopUDP2CANConvertion(third);
    StopUDP2CANConvertion(fourth);
    StopCAN2UDPConvertion(fiveth);
    StopCAN2UDPConvertion(sixth);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
