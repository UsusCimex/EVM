#include <iostream>
#include <libusb.h>
#include <stdio.h>
using namespace std;
void printdev(libusb_device* dev);
int main() {
    libusb_device** devs; // указатель на указатель на устройство,
    // используется для получения списка устройств
    libusb_context* ctx = NULL; // контекст сессии libusb
    int r; // для возвращаемых значений
    ssize_t cnt; // число найденных USB-устройств
    ssize_t i; // индексная переменная цикла перебора всех устройств
    r = libusb_init(&ctx);// инициализировать библиотеку libusb, открыть сессию работы с libusb
    if (r < 0) {
        fprintf(stderr, "Ошибка: инициализация не выполнена, код: %d.\n", r);
        return 1;
    }
    libusb_set_debug(ctx, 3); // задать уровень подробности отладочных сообщений
    cnt = libusb_get_device_list(ctx, &devs); // получить список всех найденных USB- устройств
    if (cnt < 0) {
        fprintf(stderr,"Ошибка: список USB устройств не получен.\n");
        return 1;
    }
    printf("найдено устройств: %ld\n", cnt);
    printf("===========================================================\n");
    for (i = 0; i < cnt; i++) { // цикл перебора всех устройств
        printdev(devs[i]); // печать параметров устройства
    }
    printf("===========================================================\n");
    libusb_free_device_list(devs, 1);// освободить память, выделенную функцией получения списка устройств
    libusb_exit(ctx); // завершить работу с библиотекой libusb,
    // закрыть сессию работы с libusb
    return 0;
}
void printdev(libusb_device* dev) {
    libusb_device_descriptor desc;
    libusb_device_handle *handle = nullptr;
    unsigned char str[256];

    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        fprintf(stderr, "Ошибка: дескриптор устройства не получен, код: %d.\n", r);
        return;
    }

    printf("Kласс устройства: %d\n", (int)desc.bDeviceClass);
    printf("Идентификатор производителя: %d\n", desc.idVendor);
    printf("Идентификатор устройства: %d\n", desc.idProduct);

    
    libusb_open(dev, &handle);
    if (handle) {
        if (desc.iSerialNumber) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, str, sizeof(str));
            if (r > 0) printf("Cерийный номер: %s\n", str); 
            else printf("Cерийный номер: null");
        }
        if (desc.iManufacturer) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, str, sizeof(str));
            if (r > 0) printf("Производитель: %s\n", str);
            else  printf("Производитель: null");
        }
        if (desc.iProduct) {
            r = libusb_get_string_descriptor_ascii(handle, desc.iProduct, str, sizeof(str));
            if (r > 0) printf("Наименование устройства: %s", str);
            else printf("Наименование устройства: null");
        }
    } 
    else  printf("      null");
    printf("\n");
    libusb_close(handle);
    printf("--------------------------------------------\n");
}
