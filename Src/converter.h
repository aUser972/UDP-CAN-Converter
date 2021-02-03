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


/** \brief Структура s_ConnectionParamUDP2CAN содержит параметры соединения типа UDP-CAN dev.
 */
typedef struct
{
    int port;                       /**< Номер входящего UDP порта */
    int dev;                        /**< Номер оконечного устройства на CAN шине */
    char *name;                     /**< Имя CAN интерфейса, на который отправляются данные */
    size_t size;                    /**< Размер имени интерфейса */
    pthread_mutex_t *mutexIdCAN;    /**< Указатель на mutex, блокирующий запись в сокет связанный с CAN шиной */
    void *next;                     /**< Указатель на следующий элемент списка параметров соеденения */
    int udpPackageSize;             /**< Размер UDP пакета данных */
    volatile int status;            /**< Статус соеденения */
    pid_t tid;                      /**< Идентификатор потока */
}s_ConnectionParamUDP2CAN;

/** \brief Структура s_ConnectionParamCAN2UDP содержит параметры соединения типа CAN-UDP.
 */
typedef struct
{
    char *ipAddr;                   /**< Исходящий IP адрес */
    size_t ipSize;                  /**< Размер IP адреса в байтах */
    int port;                       /**< Номер исходящего UDP порта */
    char *name;                     /**< Имя CAN интерфейса, с которого приходят данные */
    size_t nameSize;                /**< Размер имени CAN интерфейса в байтах */
    pthread_mutex_t *mutexIdUDP;    /**< Указатель на mutex, блокирующий запись в сокет связанный с UDP портом */
    void *next;                     /**< Указатель на следующий элемент списка параметров соеденения */
    volatile int status;            /**< Статус соеденения */
    pid_t tid;                      /**< Идентификатор потока */
}s_ConnectionParamCAN2UDP;

static s_ConnectionParamUDP2CAN *firstUDP2CAN = NULL; /**< Указатель на первый элемент в списке параметров подключения типа UDP-CAN dev */
static s_ConnectionParamUDP2CAN *lastUDP2CAN = NULL; /**< Указатель на последний элемент в списке параметров подключения типа UDP-CAN dev */
static s_ConnectionParamCAN2UDP *firstCAN2UDP = NULL; /**< Указатель на первый элемент в списке параметров подключения типа CAN-UDP */
static s_ConnectionParamCAN2UDP *lastCAN2UDP = NULL; /**< Указатель на последний элемент в списке параметров подключения типа CAN-UDP */

static int connectionUDP2CANCounter = 0; /**< Количество соеденений типа UDP-CAN dev */
static int connectionCAN2UDPCounter = 0; /**< Количество соеденений типа CAN-UDP */

/** \brief Функция ConverterUDP2CAN упаковывает полученные данные в структуру s_ConnectionParamUDP2CAN,
 *  записывает структуру в односвязанный список, инициализирует mutex, создает поток,
 *  вызывающий функцию CreateConnectionUDP2CAN.
 *
 * \param [in] dev_addr адрес устройства получателя на CAN шине.
 * \param [in] interface_name имя CAN интерфейса получателя.
 * \param [in] name_size длина имени CAN интерфейса в байтах..
 * \param [in] udp_pack_size длина данных UDP пакета.
 * \param [in] port_num номер UDP порта отправителя.
 * \return номер подключения, -1 - ошибка.
 *
 */
int ConverterUDP2CAN(int dev_addr, char *interface_name, size_t name_size, int udp_pack_size, int port_num);

/** \brief Функция ConverterCAN2UDP упаковывает полученные данные в структуру s_ConnectionParamCAN2UDP,
 *  записывает структуру в односвязанный список, создает поток, вызывающий функцию CreateConnectionCAN2UDP.
 *
 * \param [in] ip_addr IP адрес получателя.
 * \param [in] ip_size длина IP адреса в байтах.
 * \param [in] port_num номер UDP порта получателя.
 * \param [in] interface_name имя CAN интерфейса отправителя.
 * \param [in] name_size длина имени CAN интерфейса в байтах.
 * \return номер подключения, -1 - ошибка.
 *
 */
int ConverterCAN2UDP(char* ip_addr, size_t ip_size, int port_num, char* interface_name, size_t name_size);

/** \brief Функция CreateConnectionUDP2CAN создает UDP сокет для прослушки и CAN сокет для отправки.
 *  Данные, пришедшие в UDP порт упаковываются в CAN фрейм и отправляются соответствующему устройству на CAN шине.
 *
 * \param [in] arg указатель на структуру типа s_ConnectionParamUDP2CAN, в которой хранятся параметры подключения.
 *
 */
void *CreateConnectionUDP2CAN(void *arg);

/** \brief Функция CreateConnectionCAN2UDP создает CAN сокет для прослушки и UDP соект для отправки.
 *  Данные, пришедшие из CAN сокета записываются в UDP сокет и отправляются по IP адресу,
 *  указанному в соответствующей структуре s_ConnectionParamCAN2UDP.
 *  NOTE: Адрес CAN устройства в UDP пакет не записывается.
 *
 * \param [in] arg указатель на структуру типа s_ConnectionParamCAN2UDP, в которой хранятся параметры подключения.
 *
 */
void *CreateConnectionCAN2UDP(void *arg);

/** \brief Функция StopUDP2CANConvertion завершает работу конвертирующего соеденения типа UDP-CAN dev.
 *
 * \param [in] connectionNum номер соеденения, которое необходимо завершить.
 *
 */
void StopUDP2CANConvertion(int connectionNum);

/** \brief Функция StopCAN2UDPConvertion завершает работу конвертирующего соеденения типа CAN-UDP.
 *
 * \param [in] connectionNum номер соеденения, которое необходимо завершить.
 *
 */
void StopCAN2UDPConvertion(int connectionNum);

#endif // CONVERTER_H_INCLUDED
