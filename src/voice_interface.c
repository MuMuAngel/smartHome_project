
#if 0

struct control
{
    /* data */
    char control_name[128];     //控制模块名称
    int (*control_init)(void);  //控制模块初始化函数
    void (*final)(void);        //控制模块销毁函数
    void (*get)(void *arg);      //监听函数
    void (*set)(void *arg);      //设置函数

    struct control *next;
};

#endif
#include <stdio.h>
#include <pthread.h>
#include "voice_interface.h"
#include "msg_queue.h" //包含消息队列头文件

#include "uartTool.h" //包含串口通信头文件
#include "global.h"   //包含全局变量头文件

static int serial_fd = -1;

// 静态函数，用于初始化语音控制
static int voice_control_init(void)
{
    // 打开串口设备，并设置波特率
    serial_fd = mySerialOpen(SERIAL_DEV, BAUD);
    // 打印串口设备文件描述符
    printf("%s|%s|%d: serial_fd = %d\n", __FILE__, __func__, __LINE__, serial_fd);
    // 返回串口设备文件描述符
    return serial_fd;
}

static void voice_control_final(void)
{
    // 如果串口文件描述符不为-1
    if (-1 != serial_fd)
    {
        /* code */
        close(serial_fd);
        serial_fd = -1;
    }
}

static void *voice_control_get(void *arg)
{
    unsigned char buf[6] = {0};             // 定义一个长度为6的数组，用于存储接收到的数据
    int len = 0;                            // 定义一个变量，用于存储接收到的数据长度
    cntl_info_t *info = (cntl_info_t *)arg; // 获取控制模块信息
    mqd_t mq = info->mq;                    // 获取消息队列
    if (mq == (mqd_t)-1)
    {
        /* code */
        printf("%s|%s|%d: mq = %d\n", __FILE__, __func__, __LINE__, mq); // 打印错误信息
        pthread_exit(0);                                                 // 退出线程
    }
    if (-1 == serial_fd)
    {
        /* code */
        serial_fd = voice_control_init(); // 初始化串口
        if (-1 == serial_fd)
        {
            printf("%s|%s|%d: serial_fd = %d\n", __FILE__, __func__, __LINE__, serial_fd); // 打印错误信息
            pthread_exit(0);                                                               // 退出线程
        }
    }
    printf("%s|voice_get thread start!\n", __func__);
    pthread_detach(pthread_self()); // 分离线程
    while (1)
    {
        len = serialRecvMsg(serial_fd, buf); // 从串口读取数据
        printf("%s|%s|%d: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
               __FILE__, __func__, __LINE__, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]); // 打印接收到的数据
        printf("%s|%s|%d: len = %d\n", __FILE__, __func__, __LINE__, len);                    // 打印接收到的数据长度
        if (len > 0)
        {
            if (buf[0] == 0xAA && buf[1] == 0x55 && buf[4] == 0x55 && buf[5] == 0xAA)
            {
                printf("%s|%s|%d: buf[2] = %d\n", __FILE__, __func__, __LINE__, buf[2]); // 打印接收到的数据
                msg_queue_send(mq, buf, len);                                            // 将接收到的数据发送到消息队列
            }
            memset(buf, 0, sizeof(buf)); // 清空数组
        }
    }
}

static void *voice_control_set(void *arg)
{
    pthread_detach(pthread_self()); // 分离线程
    unsigned char *buffer = (unsigned char *)arg;

    if (buffer != NULL)
    {
        printf("%s|%s|%d: buffer = 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
               __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]); // 打印接收到的数据
        serialSendMsg(serial_fd, buffer, 6);                                                                    // 将接收到的数据发送到串口
    }
    pthread_exit(0); // 退出线程
}

struct control voice_control = {
    .control_name = "voice_control",
    .control_init = voice_control_init,
    .final = voice_control_final,
    .get = voice_control_get,
    .set = voice_control_set,
    .next = NULL,
};

// 函数：将语音添加到控制列表中
// 参数：phead：控制列表的头指针，new_ctl：要添加的语音控制结构体
// 返回值：添加后的控制列表的头指针
// 头插法
// 函数add_voice_to_cntl_list用于将语音控制添加到控制列表中
struct control *add_voice_to_cntl_list(struct control *phead)
{
    // 调用add_interface_to_cntl_list函数，将voice_control添加到控制列表中
    return add_interface_to_cntl_list(phead, &voice_control);
}