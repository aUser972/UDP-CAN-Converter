# UDP-CAN-Converter
Библиотека для конвертирования CAN фреймов в UDP пакеты и UDP пакетов в CAN фреймы.

Подробный пример использования смотрите в каталоге `./Example`



## UDP-CAN создание подключение и конвертирование
Функция `int ConverterUDP2CAN(int dev_addr, char *interface_name, size_t name_size, int udp_pack_size, int port_num)` создает подключение типа UDP-CAN и конвретирует в реальном времени данные пришедшие в UDP порт `port_num`, которые затем отправляет на указанное устройство `dev_addr` на указанном CAN интерфейсе `interface_name`.

`dev_addr` - адрес устройства на CAN шине, на которое будут отправляться данные с UDP порта.

`interface_name` - имя CAN интерфейса, в который будут отправляться данные с UDP порта.

`name_size` - длина имени CAN интерфейса.

`udp_pack_size` - длина данных UDP пакета.

`port_num` - номер UDP порта на который приходят данные для конвертации.

Возрващает номер подключения UDP-CAN.

Пример использования:
```
int first = ConverterUDP2CAN(0x1, "vcan0", 5, 1024, 5000);
```
## CAN-UDP создание подключение и конвертирование
Функция `int ConverterCAN2UDP(char* ip_addr, size_t ip_size, int port_num, char* interface_name, size_t name_size)` создает подключение типа UDP-CAN и конвретирует в реальном времени данные пришедшие из CAN интерфеса `interface_name` в UDP порт `port_num` по IP адресу `ip_addr`.

`ip_addr` - IP адрес, по которому будут отправляться пришедшие данные.

`ip_size` - длина IP адреса.

`port_num` - номер порта, по которому будут отправляться пришедшие данные.

`interface_name` - имя CAN интерфейса, с которого приходят данные.

`name_size` - длина имени CAN интерфейса.

Возрващает номер подключения CAN-UDP.

Пример использования:
```
int sixth = ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan1", 5);
```
## Закрытие UDP-CAN подключения
Функция `void StopUDP2CANConvertion(int connectionNum)` приостанавливает конвертирование и закрывает соединение типа UDP-CAN.

`connectionNum` - номер соединения, возвращаемого функцией `ConverterUDP2CAN()`.

Пример использования:
```
int first = ConverterUDP2CAN(0x1, "vcan0", 5, 1024, 5000);
...
StopUDP2CANConvertion(first);
```

## Закрытие CAN-UDP подключения
Функция `void StopCAN2UDPConvertion(int connectionNum)` приостанавливает конвертирование и закрывает соединение типа CAN-UDP.

`connectionNum` - номер соединения, возвращаемого функцией `ConverterCAN2UDP()`.

Пример использования:
```
int sixth = ConverterCAN2UDP("127.0.0.1", 9, 5004, "vcan1", 5);
...
ConverterCAN2UDP(sixth);
```
## Сборка проекта
Для сборки библиотеки перейдите в каталог `Src`:
```
cd Src
```
Затем создайте каталог для сборки и перейдите в него:
```
mkdir tmp
cd tmp
```
Из каталога tmp вызовите `cmake`, указав в качестве параметра путь до файла `CMakeLists.txt`:
```
cmake ..
```
После успешной генерации make файлов вызовите команду make:
```
make
```
В каталоге появится файл `libconverter.a`, готовый к использованию.

## Подключение библиотеки
Для подключение библиотеки в свой проект с помощью Cmake используйте следующие команды:
```
include_directories(${PATH_TO_libconverter.a})
target_link_libraries(${PROJECT_NAME} ${PATH_TO_libconverter.a}/libconverter.a)
```
