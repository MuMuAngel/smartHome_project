#ifndef __GDEVICE_H__
#define __GDEVICE_H__

#include <wiringPi.h>

struct gdevice
{
    char dev_name[128];    // 设备名称
    int key;               // key值,用于匹配控制指令的值
    int gpio_pin;          // 控制GPIO引脚
    int gpio_status;       // 高低电平设备状态
    int gpio_mode;         // GPIO输入输出模式
    int check_face_status; // 人脸识别状态
    int voice_set_status;  // 语音识别状态

    struct gdevice *next;
};

// struct gdevice *add_interface_to_device_list(struct gdevice *phead, struct gdevice *control_interface);
struct gdevice *find_device_by_key(struct gdevice *phead, int key);
struct gdevice *find_device_by_name(struct gdevice *phead, char *name);
int set_gpio_gdevice_status(struct gdevice *pdev);
#endif /* __GDEVICE_H__ */