#include "smoke_interface.h"
#include "global.h"
#include "msg_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>

#define SMOKE_PIN 6
#define SMOKE_MODE INPUT

// 静态函数，用于初始化烟雾传感器
static int smoke_init(void)
{
    // 设置烟雾传感器的引脚模式
    pinMode(SMOKE_PIN, SMOKE_MODE);
    // 返回-1，表示初始化失败
    return -1;
}

static void *smoke_get(void *arg)
{
    // 定义一个6字节的缓冲区，用于存储发送的数据
    unsigned char buffer[6] = {0xAA, 0x55, 0x45, 0x00, 0x55, 0xAA};
    // 定义一个cntl_info_t类型的指针，用于存储传入的参数
    cntl_info_t *cntl_info = NULL;
    // 定义一个mqd_t类型的变量，用于存储消息队列的描述符
    mqd_t mq = -1;
    // 定义一个整型变量，用于存储传感器的状态
    int status = HIGH;
    // 定义一个整型变量，用于存储发送的数据大小
    int send_size = 0;
    // 定义一个整型变量，用于存储烟雾传感器的信号
    int smoke_signal = 0;
    // 将当前线程分离，使其成为守护线程
    pthread_detach(pthread_self());   
    // 如果传入的参数不为空，则将其转换为cntl_info_t类型的指针
    if (arg != NULL)
    {
        /* code */
        cntl_info = (cntl_info_t *)arg;
    }
    // 如果cntl_info不为空，则获取消息队列的描述符
    if (cntl_info != NULL)
    {
        mq = cntl_info->mq;
    }
    // 如果消息队列的描述符为-1，则退出线程
    if (mq == (mqd_t)-1)
    {
        /* code */
        pthread_exit(0);
    }
    // 打印当前线程的启动信息
    printf("%s thread start\n", __func__);
    // 无限循环，持续检测烟雾传感器状态
    while (1)
    {
        // 读取烟雾传感器的状态
        status = digitalRead(SMOKE_PIN);
        // 如果传感器状态为低电平，表示检测到烟雾
        if (status == LOW)
        {
            // 设置烟雾信号为1
            smoke_signal = 1;
            // 设置缓冲区的第四个字节为0x00，表示检测到烟雾
            buffer[3] = 0x00;
            // 打印发送的数据
            printf("%s|%s|%d: send 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x to msg_queue\n", \
            __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
            // 发送数据到消息队列
            send_size = msg_queue_send(mq, buffer, sizeof(buffer));
            // 如果发送失败，则继续下一次循环
            if (send_size == -1)
            {
                continue;
            }
        }
        // 如果传感器状态为高电平，且烟雾信号为1，表示烟雾已经消失
        else if (status == HIGH && smoke_signal == 1)
        {
            // 设置烟雾信号为0
            smoke_signal = 0;
            // 设置缓冲区的第四个字节为0x01，表示烟雾已经消失
            buffer[3] = 0x01;
            // 打印发送的数据
            printf("%s|%s|%d: send 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x to msg_queue\n", \
                __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
            // 发送数据到消息队列
            send_size = msg_queue_send(mq, buffer, sizeof(buffer));
            // 如果发送失败，则继续下一次循环
            if (send_size == -1)
            {
                continue;
            }
        }
        // 每隔4秒检测一次烟雾传感器状态
        sleep(4);
    }
    // 退出线程
    pthread_exit(0);
}

struct control smoke_control =
{
    /* data */
    .control_name = "smoke",
    .control_init = smoke_init,
    .final = NULL,
    .get = smoke_get,
    .set = NULL,
    .next = NULL, // smoke_next,
};

// 函数：向控制列表中添加烟雾控制
// 参数：控制列表的头指针
// 返回值：更新后的控制列表的头指针
// 函数add_smoke_to_cntl_list用于将smoke_control添加到cntl_info中
struct control *add_smoke_to_cntl_list(struct control *phead)
{
    // 调用add_interface_to_cntl_list函数，将smoke_control添加到cntl_info中
    return add_interface_to_cntl_list(phead, &smoke_control);
}