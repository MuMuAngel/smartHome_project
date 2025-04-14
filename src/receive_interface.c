#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>

#include "global.h"
#include "receive_interface.h"
#include "myoled.h"
#include "face.h"
#include "control.h"
#include "msg_queue.h"
#include "gdevice.h"
#include "ini.h"

typedef struct
{
    int len;
    unsigned char *buffer;
    cntl_info_t *info;
} recv_msg_t;

struct oled_str_t
{
    char dev_name[128];
    char msg[128];
};

static struct gdevice *pdevhead = NULL;

static int oled_fd = -1; // 全局变量，用于保存OLED的文件描述符

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
static int handler(void *user, const char *section, const char *name,
                   const char *value)
{
    // 定义一个指向gdevice结构体的指针
    struct gdevice *pdev = NULL;
    // 如果pdevhead为空
    if (pdevhead == NULL)
    {
        // 分配内存给pdevhead
        pdevhead = (struct gdevice *)malloc(sizeof(struct gdevice));
        // 将pdevhead清零
        memset(pdevhead, 0, sizeof(struct gdevice));
        // 将section的值复制给pdevhead的dev_name
        strcpy(pdevhead->dev_name, section);
    }
    // 如果pdevhead不为空且section的值与pdevhead的dev_name不相等
    else if (strcmp(pdevhead->dev_name, section) != 0)
    {
        // 分配内存给pdev
        pdev = (struct gdevice *)malloc(sizeof(struct gdevice));
        // 将pdev清零
        memset(pdev, 0, sizeof(struct gdevice));
        // 将section的值复制给pdev的dev_name
        strcpy(pdev->dev_name, section);
        // 将pdev的next指针指向pdevhead
        pdev->next = pdevhead;
        // 将pdevhead指向pdev
        pdevhead = pdev;
    }

    // 如果pdevhead不为空
    if (pdevhead != NULL)
    {
        // 如果pdevhead的dev_name与"key"相等
        if (MATCH(pdevhead->dev_name, "key"))
        {
            // 将value的值转换为整数并赋给pdevhead的key
            sscanf(value, "%x", &pdevhead->key);
            // 打印pdevhead的key
            // printf("%d|pdevhead->key = 0x%x\n", __LINE__, pdevhead->key);
        }
        // 如果pdevhead的dev_name与"gpio_pin"相等
        else if (MATCH(pdevhead->dev_name, "gpio_pin"))
        {
            // 将value的值转换为整数并赋给pdevhead的gpio_pin
            pdevhead->gpio_pin = atoi(value);
            // 打印pdevhead的gpio_pin
            // printf("%d|pdevhead->gpio_pin = %d\n", __LINE__, pdevhead->gpio_pin);
        }
        // 如果pdevhead的dev_name与"gpio_mode"相等
        else if (MATCH(pdevhead->dev_name, "gpio_mode"))
        {
            // 如果value的值与"INPUT"相等
            if (strcmp(value, "INPUT") == 0)
            {
                // 将pdevhead的gpio_mode赋值为INPUT
                pdevhead->gpio_mode = INPUT;
            }
            // 如果value的值与"OUTPUT"相等
            else if (strcmp(value, "OUTPUT") == 0)
            {
                // 将pdevhead的gpio_mode赋值为OUTPUT
                pdevhead->gpio_mode = OUTPUT;
            }
            // 打印pdevhead的gpio_mode
            // printf("%d|pdevhead->gpio_mode = %d\n", __LINE__, pdevhead->gpio_mode);
        }
        // 如果pdevhead的gpio_status与"gpio_status"相等
        else if (MATCH(pdevhead->dev_name, "gpio_status"))
        {
            // 如果value的值与"LOW"相等
            if (strcmp(value, "LOW") == 0)
            {
                // 将pdevhead的gpio_status赋值为LOW
                pdevhead->gpio_status = LOW;
            }
            // 如果value的值与"HIGH"相等
            else if (strcmp(value, "HIGH") == 0)
            {
                // 将pdevhead的gpio_status赋值为HIGH
                pdevhead->gpio_status = HIGH;
            }
            // 打印pdevhead的gpio_status
            // printf("%d|pdevhead->gpio_status = %d\n", __LINE__, pdevhead->gpio_status);
        }
        // 如果pdevhead的check_face_status与"check_face_status"相等
        else if (MATCH(pdevhead->dev_name, "check_face_status"))
        {
            // 将value的值转换为整数并赋给pdevhead的check_face_status
            pdevhead->check_face_status = atoi(value);
            // 打印pdevhead的check_face_status
            // printf("%d|pdevhead->check_face_status = %d\n", __LINE__, pdevhead->check_face_status);
        }
        // 如果pdevhead的voice_set_status与"voice_set_status"相等
        else if (MATCH(pdevhead->dev_name, "voice_set_status"))
        {
            // 将value的值转换为整数并赋给pdevhead的voice_set_status
            pdevhead->voice_set_status = atoi(value);
            // 打印pdevhead的voice_set_status
            // printf("%d|pdevhead->voice_set_status = %d\n", __LINE__, pdevhead->voice_set_status);
        }
    }
    // 返回1
    return 1;
}

// 静态函数，用于初始化接收信息
static int receive_init(void)
{
    struct oled_str_t *oledstr = NULL;
    // TODO: 实现接收信息的初始化
    if (ini_parse("/etc/gdevice.ini", handler, NULL) < 0)
    {
        printf("Can't load 'test.ini'\n");
        // return 1;
    }
    struct gdevice *pdev = pdevhead; // 定义一个指向gdevice结构体的指针
    oledstr = (struct oled_str_t *)malloc(sizeof(struct oled_str_t));
    // 将oledstr清零
    memset(oledstr, 0, sizeof(struct oled_str_t));
    // 初始化oled
    oled_fd = myoled_init();
    face_init();
    while (pdev)
    {
        printf("**********************************************************\n");
        printf("pdev->dev_name = %s\n", pdev->dev_name);
        printf("pdev->key = 0x%x\n", pdev->key);
        printf("pdev->gpio_pin = %d\n", pdev->gpio_pin);
        printf("pdev->gpio_mode = %d\n", pdev->gpio_mode);
        printf("pdev->gpio_status = %d\n", pdev->gpio_status);
        printf("pdev->check_face_status = %d\n", pdev->check_face_status);
        printf("pdev->voice_set_status = %d\n", pdev->voice_set_status);

        pdev = pdev->next;
    }

    return oled_fd;
}
// 静态函数，用于接收最终数据
static void receive_final(void)
{
    // 调用face_final函数
    face_final();
    // 如果oled_fd不为-1，则关闭oled_fd，并将oled_fd置为-1
    if (oled_fd != -1)
    {
        close(oled_fd);
        oled_fd = -1;
    }
}

static void *device_handle(void *arg)
{
    // 分离线程
    pthread_detach(pthread_self());
    pthread_t tid;
    // 定义设备状态、烟雾状态、人脸识别结果
    int gdevice_status = -1;
    int smoke_status = -1;
    char success_or_failed[20] = "success";
    double face_result = 0.00;
    // 定义接收消息结构体
    recv_msg_t *recv_msg = NULL;
    struct gdevice *cur_gdev = NULL;
    struct gdevice *pfan_gdev = NULL;
    struct control *pcntl = NULL;
    struct oled_str_t *oledstr = NULL;
    if (arg != NULL)
    {
        recv_msg = (recv_msg_t *)arg;
        printf("%s|%s|%d: receive %d byte from msg_queue\n", __FILE__, __func__, __LINE__, recv_msg->len);
        printf("msg is 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
               recv_msg->buffer[0], recv_msg->buffer[1], recv_msg->buffer[2], recv_msg->buffer[3], recv_msg->buffer[4], recv_msg->buffer[5]);
    }

    if (recv_msg != NULL && recv_msg->buffer != NULL)
    {
        printf("%s|%s|%d: buffer[2] = 0x%x\n", __FILE__, __func__, __LINE__, recv_msg->buffer[2]);
        cur_gdev = find_device_by_key(pdevhead, recv_msg->buffer[2]);
    }

    if (cur_gdev != NULL)
    {
        oledstr = (struct oled_str_t *)malloc(sizeof(struct oled_str_t));
        memset(oledstr, 0, sizeof(struct oled_str_t));
        strcpy(oledstr->dev_name, cur_gdev->dev_name);
        cur_gdev->gpio_status = recv_msg->buffer[3] == 0x00 ? LOW : HIGH;

        printf("%s|%s|%d: cur_gdev->gpio_status = %d\n", __FILE__, __func__, __LINE__, cur_gdev->gpio_status);
#if 0
        gdevice_status = set_gpio_gdevice_status(cur_gdev);
        if (gdevice_status == 0)
        {
            printf("%s|%s|%d: set gpio status success\n", __FILE__, __func__, __LINE__);
        }
        else
        {
            printf("%s|%s|%d: set gpio status failed\n", __FILE__, __func__, __LINE__);
        }
#endif
        if (cur_gdev->check_face_status == 1)
        {
            face_result = face_data();
            printf("%s|%s|%d: face_result = %lf\n", __FILE__, __func__, __LINE__, face_result);
            if (face_result > 0.55)
            {
                recv_msg->buffer[2] = 0x47;
                gdevice_status = set_gpio_gdevice_status(cur_gdev);
                printf("%s|%s|%d: unlock success!\n", __FILE__, __func__, __LINE__);
            }
            else
            {
                recv_msg->buffer[2] = 0x46;
                gdevice_status = -1;
            }
        }
        else
        {
            gdevice_status = set_gpio_gdevice_status(cur_gdev);
        }
        // 如果是烟雾传感器
        if (cur_gdev->voice_set_status == 1)
        {
            if (recv_msg != NULL && recv_msg->info != NULL && recv_msg->info->cntl_phead != NULL)
            {
                pcntl = recv_msg->info->cntl_phead;
                while (pcntl != NULL)
                {
                    if (strstr(pcntl->control_name, "voice_control") != NULL)
                    {
                        printf("%s|%s|%d: voice_control recv_msg->buffer[2] = 0x%x\n", __FILE__, __func__, __LINE__, recv_msg->buffer[2]);
                        if (recv_msg->buffer[3] == 0x00 && recv_msg->buffer[2] == 0x45)
                        {
                            smoke_status = 1;
                            // recv_msg->buffer[3] = 0x01;
                        }
                        else
                        {
                            smoke_status = 0;
                            // recv_msg->buffer[3] = 0x00;
                        }
                        pthread_create(&tid, NULL, pcntl->set, (void *)recv_msg->buffer);
                        break;
                    }
                    pcntl = pcntl->next;
                }
            }
        }

        if (gdevice_status == 0)
        {
            printf("%s|%s|%d: set gpio status success\n", __FILE__, __func__, __LINE__);
        }
        else
        {
            printf("%s|%s|%d: set gpio status failed\n", __FILE__, __func__, __LINE__);
            memset(success_or_failed, '\0', sizeof(success_or_failed));
            strncpy(success_or_failed, "failed", 6);
        }
#if 0
        // oled屏显示
        char oled_msg[512];
        memset(oled_msg, 0, sizeof(oled_msg));
        char *change_status = cur_gdev->gpio_status == LOW ? "Open" : "Close";
        sprintf(oled_msg, "%s %s %s!\n", change_status, cur_gdev->dev_name, success_or_failed);
        // special for smoke
        if (smoke_status == 1)
        {
            memset(oled_msg, 0, sizeof(oled_msg));
            strcpy(oled_msg, "A risk of fire!\n");
        }

        printf("oled_msg=%s\n", oled_msg);
        oled_show(oled_msg);
#endif
        // 发送消息到oled显示
        char *change_status = cur_gdev->gpio_status == LOW ? "on" : "off";
        strcpy(oledstr->msg, change_status);
        if (smoke_status == 1 && recv_msg->buffer[2] == 0x45 && recv_msg->buffer[3] == 0x00)
        {
            strcpy(oledstr->msg, "A risk of fire!\n");
            pfan_gdev = find_device_by_name(pdevhead, "fan");
            if (pfan_gdev != NULL)
            {
                pfan_gdev->gpio_status = LOW;
                gdevice_status = set_gpio_gdevice_status(pfan_gdev);
                if (gdevice_status == 0)
                {
                    printf("%s|%s|%d: open fan success\n", __FILE__, __func__, __LINE__);
                }
                else
                {
                    printf("%s|%s|%d: open fan failed\n", __FILE__, __func__, __LINE__);
                }
            }
        }
        else if (smoke_status == 0 && recv_msg->buffer[2] == 0x45 && recv_msg->buffer[3] == 0x01)
        {
            pfan_gdev = find_device_by_name(pdevhead, "fan");
            if (pfan_gdev != NULL)
            {
                pfan_gdev->gpio_status = HIGH;
                gdevice_status = set_gpio_gdevice_status(pfan_gdev);
                if (gdevice_status == 0)
                {
                    printf("%s|%s|%d: close fan success\n", __FILE__, __func__, __LINE__);
                }
                else
                {
                    printf("%s|%s|%d: close fan failed\n", __FILE__, __func__, __LINE__);
                }
            }
        }

        printf("oled_msg=%s\n", oledstr->msg);
        oled_show((void *)oledstr);

        if (cur_gdev->check_face_status == 1 && face_result > 0.55 && gdevice_status == 0)
        {
            sleep(4);
            cur_gdev->gpio_status = HIGH;
            gdevice_status = set_gpio_gdevice_status(cur_gdev);
            if (gdevice_status == 0)
            {
                printf("%s|%s|%d: lock success\n", __FILE__, __func__, __LINE__);
            }
            else
            {
                printf("%s|%s|%d: lock failed\n", __FILE__, __func__, __LINE__);
            }
            char *change_status = cur_gdev->gpio_status == LOW ? "on" : "off";
            strcpy(oledstr->msg, change_status);
            oled_show((void *)oledstr);
        }
    }
    free(oledstr);
    pthread_exit(0);
}

static void *receive_get(void *arg)

{
    // 分离线程
    pthread_detach(pthread_self());
    pthread_t tid;
    // 定义缓冲区
    unsigned char *buffer = NULL;
    // 定义接收消息结构体
    recv_msg_t *recv_msg = NULL;
    // 定义消息队列属性
    struct mq_attr attr;
    // 如果参数不为空
    if (arg != NULL)
    {
        // 分配内存
        recv_msg = (recv_msg_t *)malloc(sizeof(recv_msg_t));
        recv_msg->info = (cntl_info_t *)arg; // 获取到mq和phead(struct control *phead链表)
        recv_msg->len = -1;
        recv_msg->buffer = NULL;
    }
    else
    {
        pthread_exit(0);
    }

    if (mq_getattr(recv_msg->info->mq, &attr) == -1)
    {
        printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
        perror("mq_getattr");
        pthread_exit(0);
    }
    recv_msg->buffer = (unsigned char *)malloc(attr.mq_msgsize);
    buffer = (unsigned char *)malloc(attr.mq_msgsize);
    memset(recv_msg->buffer, 0, attr.mq_msgsize);
    memset(buffer, 0, attr.mq_msgsize);

    printf("%s thread start\n", __func__);
    while (1)
    {
        recv_msg->len = mq_receive(recv_msg->info->mq, (char *)buffer, attr.mq_msgsize, NULL);
        printf("%s|%s|%d: receive %d byte from msg_queue\n", __FILE__, __func__, __LINE__, recv_msg->len);
        printf("msg is 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
               buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
        if (recv_msg->len == -1)
        {
            if (errno == EAGAIN)
            {
                printf("%s|%s|%d: msg_queue is empty\n", __FILE__, __func__, __LINE__);
                continue;
            }
            else
            {
                printf("%s|%s|%d\n", __FILE__, __func__, __LINE__);
                perror("mq_receive");
                break;
            }
        }
        else if (buffer[0] == 0xAA && buffer[1] == 0x55 && buffer[4] == 0x55 && buffer[5] == 0xAA)
        {
            memcpy(recv_msg->buffer, buffer, recv_msg->len);
            pthread_create(&tid, NULL, (void *)device_handle, (void *)recv_msg);
        }
    }

    pthread_exit(0);
}

static void *receive_set(void *arg)
{
}

struct control receive_control =
    {
        /* data */
        .control_name = "receive_control",
        .control_init = receive_init,
        .final = receive_final,
        .get = receive_get,
        .set = receive_set,
        .next = NULL};

struct control *add_receive_to_cntl_list(struct control *phead)
{
    return add_interface_to_cntl_list(phead, &receive_control);
}
