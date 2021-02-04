#include "converter.h"

int ConverterUDP2CAN(int dev_addr, char *interface_name, size_t name_size, int udp_pack_size, int port_num)
{

    s_ConnectionParamUDP2CAN *conn;
    s_ConnectionParamUDP2CAN *tmp = firstUDP2CAN;
    conn = calloc(sizeof(s_ConnectionParamUDP2CAN), sizeof(char));
    conn->name = calloc(name_size, sizeof(char));
    conn->port = port_num;
    conn->dev = dev_addr;
    conn->size = name_size;
    conn->udpPackageSize = udp_pack_size;
    memcpy(conn->name, interface_name, name_size);
    //Поиск открытых соеденений на таком же интерфейсе, чтобы юзать один mutex
    for(int i=0; i<connectionUDP2CANCounter;i++)
    {
            if(0 == strcmp(tmp->name, interface_name))
            {
                conn->mutexIdCAN = tmp->mutexIdCAN;
                break;
            }
            tmp = (s_ConnectionParamUDP2CAN*)(tmp->next);
    }
    if(0 == connectionUDP2CANCounter)
    {
        firstUDP2CAN = conn;
        lastUDP2CAN = conn;
    }
    else {
        lastUDP2CAN->next = (void*)conn;
        lastUDP2CAN = conn;
    }

    if(!conn->mutexIdCAN)
    {
        conn->mutexIdCAN = malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(conn->mutexIdCAN, NULL);
    }

    if(pthread_create(&conn->tid, NULL, CreateConnectionUDP2CAN, conn))
    {
        perror("Error while create thread");
        return -1;
    }
    connectionUDP2CANCounter++;
    return connectionUDP2CANCounter;
}



int ConverterCAN2UDP(char* ip_addr, size_t ip_size, int port_num, char* interface_name, size_t name_size)
{

    s_ConnectionParamCAN2UDP *conn;
    s_ConnectionParamCAN2UDP *tmp = firstCAN2UDP;
    conn = calloc(sizeof(s_ConnectionParamCAN2UDP), sizeof(char));
    conn->ipAddr = calloc(ip_size, sizeof(char));
    conn->name = calloc(name_size, sizeof(char));
    conn->port = port_num;
    conn->ipSize = ip_size;
    conn->nameSize = name_size;
    memcpy(conn->ipAddr, ip_addr, ip_size);
    memcpy(conn->name, interface_name, name_size);

    //Поиск открытых соеденений на таком же интерфейсе, чтобы юзать один mutex
    for(int i=0; i<connectionCAN2UDPCounter;i++)
    {
            if(tmp->port == conn->port)
            {
                conn->mutexIdUDP = tmp->mutexIdUDP;
                break;
            }
            tmp = (s_ConnectionParamCAN2UDP*)(tmp->next);
    }

    if(0 == connectionCAN2UDPCounter)
    {
        firstCAN2UDP = conn;
        lastCAN2UDP = conn;
    }
    else {
        lastCAN2UDP->next = (void*)conn;
        lastCAN2UDP = conn;
    }

    if(!conn->mutexIdUDP)
    {
        conn->mutexIdUDP = malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(conn->mutexIdUDP, NULL);
    }

    if(pthread_create(&conn->tid, NULL, CreateConnectionCAN2UDP, conn))
    {
        perror("Error while create thread");
        return -1;
    }
    connectionCAN2UDPCounter++;
    return connectionCAN2UDPCounter;
}

void *CreateConnectionUDP2CAN(void *arg)
{
    s_ConnectionParamUDP2CAN *connection = (s_ConnectionParamUDP2CAN *)arg;
    connection->status = 1;
    errno = 0;
    struct ifreq ifr;
    struct sockaddr_can addrCAN;
    struct sockaddr_in addrIn;
    struct can_frame frame;
    int sockCAN = 0;
    int sockIn = 0;
    int nbytes;
    char *udpBuffer = calloc(connection->udpPackageSize, sizeof(char));

    sockCAN = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(-1 == sockCAN)
    {
        perror("Error while create CAN socket");
        return;
    }
    sockIn = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == sockIn)
    {
        perror("Error while create input socket");
        return;
    }
    memset(&ifr, 0 , sizeof(struct ifreq));
    memset(&addrCAN, 0 , sizeof(struct sockaddr_can));
    memset(&addrIn, 0, sizeof(struct sockaddr));
    memset(&frame, 0, sizeof(struct can_frame));

    memcpy(ifr.ifr_name, connection->name, 5);
    ioctl(sockCAN, SIOCGIFINDEX, &ifr);

    addrCAN.can_family = AF_CAN;
    addrCAN.can_ifindex = ifr.ifr_ifindex;
    addrIn.sin_family = AF_INET;
    addrIn.sin_port = htons(connection->port);
    addrIn.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(sockIn, (struct sockaddr*)&addrIn, sizeof(struct sockaddr)))
    {
        perror("Error while bind sockIn");
        return;
    }
    if(-1 == bind(sockCAN, (struct sockaddr*)&addrCAN, sizeof(struct sockaddr_can)))
    {
        perror("Error while bind sockCan");
        return;
    }

    frame.can_id = connection->dev;
    frame.can_dlc = 8;



    while(connection->status)
    {
        nbytes =read(sockIn, udpBuffer, connection->udpPackageSize);
        LOG_ARG("Bytes read %d\n", nbytes);
        pthread_mutex_lock(connection->mutexIdCAN);
        int wbyte = 0;
        int j = 0;
        while(nbytes>0)
        {
            wbyte = (nbytes > 8) ? 8 : nbytes;
            memcpy(frame.data, &udpBuffer[j], wbyte);
            int nbytes2 = write(sockCAN, &frame, sizeof(struct can_frame));
            LOG_ARG("Write data i: %d byte: %d\n", j, wbyte);
            j=j+wbyte;
            nbytes=nbytes-8;
        }
        write(sockCAN, &frame, sizeof(struct can_frame));
        pthread_mutex_unlock(connection->mutexIdCAN);
        nbytes = 0;
    }
    free(connection);
}

void *CreateConnectionCAN2UDP(void *arg)
{
    s_ConnectionParamCAN2UDP *connection = (s_ConnectionParamCAN2UDP *)arg;
    connection->status = 1;
    errno = 0;
    struct ifreq ifr;
    struct sockaddr_can addrCAN;
    struct sockaddr_in addrOut;
    struct can_frame frame;
    int sockCAN = 0;
    int sockOut = 0;
    int nbytes;


    sockCAN = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(-1 == sockCAN)
    {
        perror("Error while create CAN socket");
        return;
    }
    sockOut = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == sockOut)
    {
        perror("Error while create output socket");
        return;
    }

    memset(&ifr, 0 , sizeof(struct ifreq));
    memset(&addrCAN, 0 , sizeof(struct sockaddr_can));
    memset(&addrOut, 0, sizeof(struct sockaddr));
    memset(&frame, 0, sizeof(struct can_frame));

    memcpy(ifr.ifr_name, connection->name, connection->nameSize);
    ioctl(sockCAN, SIOCGIFINDEX, &ifr);

    addrCAN.can_family = AF_CAN;
    addrCAN.can_ifindex = ifr.ifr_ifindex;
    addrOut.sin_family = AF_INET;
    addrOut.sin_addr.s_addr = inet_addr(connection->ipAddr);
    addrOut.sin_port = htons(connection->port);

    if(-1 == bind(sockCAN, (struct sockaddr*)&addrCAN, sizeof(struct sockaddr_can)))
    {
        perror("Error while bind sockCan");
        return;
    }

    if(-1 == connect(sockOut, (struct sockaddr*)&addrOut, sizeof(struct sockaddr)))
    {
        perror("Error while connect sockOut");
        return;
    }

    while(connection->status)
    {
        nbytes = read(sockCAN, &frame, sizeof(struct can_frame));
        LOG_ARG("Read bytes %d\n", nbytes);
        pthread_mutex_lock(connection->mutexIdUDP);
        write(sockOut, frame.data, sizeof(frame.data));
        pthread_mutex_unlock(connection->mutexIdUDP);
    }
    free(connection);
}

void StopUDP2CANConvertion(int connectionNum)
{
    s_ConnectionParamUDP2CAN *tmp = firstUDP2CAN;
    for(int i=1; i < connectionNum; i++)
    {
        tmp = (s_ConnectionParamUDP2CAN*)(tmp->next);
    }
    tmp->status = 0;
}

void StopCAN2UDPConvertion(int connectionNum)
{
    s_ConnectionParamCAN2UDP *tmp = firstCAN2UDP;
    for(int i=1; i<connectionNum; i++)
    {
        tmp = (s_ConnectionParamCAN2UDP*)(tmp->next);
    }
    tmp->status = 0;
}
