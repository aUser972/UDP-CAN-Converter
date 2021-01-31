#ifndef CONVERTER_H_INCLUDED
#define CONVERTER_H_INCLUDED

#include <stdio.h>
#include <linux/can.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdlib.h>

#ifdef DEBUG
#define LOG_ARG(format, ...) {printf(format, __VA_ARGS__);}
#define LOG(format) {printf(format);}
#else
#define LOG_ARG(format, ...) {}
#define LOG(format) {}
#endif // DEBUG



typedef struct
{
    int port;
    int dev;
    char *name;
    size_t size;
    pthread_mutex_t *mutexIdCAN;
    void *next;
    int udpPackageSize;
    volatile int status;
}s_ConnectionParamUDP2CAN;

typedef struct
{
    char *ipAddr;
    size_t ipSize;
    int port;
    char *name;
    size_t nameSize;
    void *next;
    volatile int status;
}s_ConnectionParamCAN2UDP;

static s_ConnectionParamUDP2CAN *firstUDP2CAN = NULL;
static s_ConnectionParamUDP2CAN *lastUDP2CAN = NULL;
static s_ConnectionParamCAN2UDP *firstCAN2UDP = NULL;
static s_ConnectionParamCAN2UDP *lastCAN2UDP = NULL;

static connectionUDP2CANCounter = 0;
static connectionCAN2UDPCounter = 0;

/** \brief Функция ConverterUDP2CAN упаковывает полученные данные в структуру s_ConnectionParamUDP2CAN,
 *  создает поток, в котором вызывает функцию создания
 *
 * \param
 * \param
 * \return
 *
 */
int ConverterUDP2CAN(int port_num, int dev_addr, char *interface_name, size_t name_size, int udp_pack_size);
int ConverterCAN2UDP(char* ip_addr, size_t ip_size, int port_num, char* interface_name, size_t name_size);
void *CreateConnectionUDP2CAN(void *arg);
void *CreateConnectionCAN2UDP(void *arg);
void StopUDP2CANConvertion(int connectionNum);
void StopCAN2UDPConvertion(int connectionNum);

#endif // CONVERTER_H_INCLUDED
