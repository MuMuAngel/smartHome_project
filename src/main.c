#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>

#include "face.h"
#include "control.h"
#include "gdevice.h"
#include "voice_interface.h"
#include "socket_interface.h"
#include "smoke_interface.h"
#include "receive_interface.h"
#include "msg_queue.h"
#include "myoled.h"
#include "global.h"

int main(int argc, char *argv[])
{
    // 定义节点数量
    int node_num = 0;

    // 定义线程ID
    // pthread_t tid;
    // 定义控制链表头指针
    struct control *control_phead = NULL;
    // 定义控制链表指针
    struct control *control_point = NULL;
    // 分配cntl_info_t结构体空间
    cntl_info_t *cntl_info = (cntl_info_t *)malloc(sizeof(cntl_info_t));
    // 初始化cntl_info_t结构体
    cntl_info->cntl_phead = NULL;

    if (wiringPiSetup() == -1) // 初始化wiringPi
    {
        /* code */
        perror("wiringPiSetup error\n");
        return -1;
    }

    cntl_info->mq = msg_queue_init();
    // 初始化消息队列
    if (cntl_info->mq == -1)
    {
        /* code */
        perror("msg_queue_init error\n");
        return -1;
    }
    // 添加语音到控制链表
    cntl_info->cntl_phead = add_voice_to_cntl_list(cntl_info->cntl_phead);
    cntl_info->cntl_phead = add_socket_to_cntl_list(cntl_info->cntl_phead);
    cntl_info->cntl_phead = add_smoke_to_cntl_list(cntl_info->cntl_phead);
    cntl_info->cntl_phead = add_receive_to_cntl_list(cntl_info->cntl_phead);
    // 遍历控制链表
    control_point = cntl_info->cntl_phead;
    while (control_point != NULL)
    {
        /* code */
        // 初始化控制节点
        if (control_point->control_init != NULL)
        {
            printf("%s|%s|%d: control_point->name = %s\n",
                   __FILE__, __func__, __LINE__, control_point->control_name);
            control_point->control_init();
        }
        // 移动到下一个节点
        control_point = control_point->next;
        // 节点数量加一
        node_num++;
    }
    // 分配线程ID数组空间
    pthread_t *tid = (pthread_t *)malloc(sizeof(int) * node_num);
    // 重置控制链表指针
    control_point = cntl_info->cntl_phead;
    // 创建线程
    for (int i = 0; i < node_num; i++)
    {
        /* code */
        // 如果节点有get函数，则创建线程
        if (control_point->get != NULL)
        {
            printf("%s|%s|%d: control_point->name = %s\n",
                   __FILE__, __func__, __LINE__, control_point->control_name);
            pthread_create(&tid[i], NULL, (void *)control_point->get, (void *)cntl_info);
        }
        // 移动到下一个节点
        control_point = control_point->next;
    }

    // 等待线程结束
    for (int i = 0; i < node_num; i++)
    {
        /* code */
        pthread_join(tid[i], NULL);
    }

    // 遍历控制链表
    for (int i = 0; i < node_num; i++)
    {
        // 执行控制节点的final函数
        if (control_point->final != NULL)
        {
            control_point->final();
        }
        // 移动到下一个节点
        control_point = control_point->next;
    }

    // 销毁消息队列
    msg_queue_destroy(cntl_info->mq);

    // // 定义结果变量
    // double result = 0.00;
    // // 初始化人脸识别
    // face_init();
    // // 获取人脸识别数据
    // result = face_data();
    // // 结束人脸识别
    // face_final();
    // // 打印结果
    // printf("result = %lf\n", result);
    free(cntl_info);
    return 0;
}