#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>

#include "socket_interface.h"
#include "msg_queue.h"
#include "global.h"

int s_fd = -1;
struct mq_fd
{
    /* data */
    mqd_t mq;
    int fd;
};

// 函数：tcp_socket_init
// 功能：初始化TCP套接字
int tcp_socket_init(void)
{
    // 初始化TCP套接字
    s_fd = socket_init(IPADDR, IPPORT);
    // 如果套接字初始化失败
    if (s_fd < 0)
    {
        /* code */
        // 打印错误信息
        printf("%s|%s|%d:socket_init failed!\n", __FILE__, __func__, __LINE__);
    }
    // 返回套接字描述符
    return s_fd;
}
// 函数：tcp_socket_final
// 功能：关闭套接字
void tcp_socket_final(void)
{
    // 关闭套接字
    close(s_fd);
}

// 函数：pread_socket
// 功能：接收数据
void *pread_socket(void *arg)
{
    // 定义一个指针，指向mq_fd结构体
    struct mq_fd *mq_fd = NULL;
    // 将arg强制转换为mq_fd结构体指针
    mq_fd = (struct mq_fd *)arg;
    // 定义一个缓冲区，大小为BUFFER_SIZE
    unsigned char buffer[BUFFER_SIZE] = {0};
    // 定义一个变量，用于存储读取的字节数
    int n_read;

    // 将缓冲区清零
    memset(buffer, 0, sizeof(buffer));
    // 循环读取数据
    while ((n_read = recv(mq_fd->fd, buffer, sizeof(buffer), 0)) > 0)
    {
        // 打印读取的字节数
        printf("read %d bytes from socket_client\n", n_read);
        // 打印缓冲区中的数据
        printf("%s|%s|%d: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
               __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
        // 如果读取的字节数大于0
        if (n_read > 0)
        {
            // 如果缓冲区中的数据符合要求
            if (buffer[0] == 0xAA && buffer[1] == 0x55 && buffer[4] == 0x55 && buffer[5] == 0xAA)
            {
                // 打印发送的数据
                printf("%s|%s|%d: send 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x to msg_queue\n",
                       __FILE__, __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
                // 将数据发送到消息队列
                msg_queue_send(mq_fd->mq, buffer, n_read);
            }
        }
        // 如果读取的字节数小于等于0
        else
        {
            // 打印接收失败的信息
            printf("%s|%s|%d: recv failed!\n", __FILE__, __func__, __LINE__);
            // 跳出循环
            break;
        }
    }
    // 如果读取的字节数等于0
    if (n_read == 0)
    {
        // 打印客户端断开连接的信息
        printf("client %d disconnect\n", mq_fd->fd);
        // 关闭文件描述符
        close(mq_fd->fd);
        // 退出线程
        pthread_exit(0);
    }

    // 关闭文件描述符
    close(mq_fd->fd);
    // 退出线程
    pthread_exit(0);
}
// 函数：tcp_socket_get
// 功能：接收客户端连接
static void *tcp_socket_get(void *arg)
{
    // int mq_fd->fd;
    //    int n_read = 0;
    int len = 0;
    int ret = -1;
    //    char buffer[6] = {'\0'};
    pthread_t pread_tid;
    cntl_info_t *cntl_info = NULL;
    struct sockaddr_in client_addr;
    struct mq_fd *mq_fd = NULL;
    // 分配内存空间
    mq_fd = (struct mq_fd *)malloc(sizeof(struct mq_fd));
    cntl_info = (cntl_info_t *)malloc(sizeof(cntl_info_t));
    // 将arg转换为cntl_info_t类型
    cntl_info = (cntl_info_t *)arg;
    // 将mq赋值给mq_fd
    mq_fd->mq = cntl_info->mq;

    // 分离线程
    pthread_detach(pthread_self());

    // 设置TCP保活机制
    int keepalive = 1;
    int keepidle = 5;
    int keepcnt = 3;
    int keepintvl = 3;

    printf("%s|%s|%d: s_fd = %d\n", __FILE__, __func__, __LINE__, s_fd);
    // 如果s_fd小于0，则初始化TCP套接字
    if (-1 == s_fd)
    {
        s_fd = tcp_socket_init();
        if (s_fd < 0)
        {
            /* code */
            printf("%s|%s|%d:tcp_socket_init failed!\n", __FILE__, __func__, __LINE__);
            pthread_exit(0);
        }
    }
    // 清空client_addr结构体
    memset(&client_addr, 0, sizeof(client_addr));
    len = sizeof(struct sockaddr_in);
    printf("%s thread start!\n", __func__);
    // 无限循环，等待客户端连接
    while (1)
    {
        /* code */
        // 接受客户端连接
        mq_fd->fd = accept(s_fd, (struct sockaddr *)&client_addr, &len);
        if (mq_fd->fd < 0)
        {
            perror("accept client failed");
            close(mq_fd->fd);
            continue;
        }
        // 设置TCP保活机制
        ret = setsockopt(mq_fd->fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
        if (ret < 0)
        {
            /* code */
            printf("%s|%s|%d: setsockopt failed!\n", __FILE__, __func__, __LINE__);
            perror("setsockopt failed");
            break;
        }
        ret = setsockopt(mq_fd->fd, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle));
        if (ret < 0)
        {
            /* code */
            printf("%s|%s|%d: setsockopt failed!\n", __FILE__, __func__, __LINE__);
            perror("setsockopt failed");
            break;
        }
        ret = setsockopt(mq_fd->fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof(keepcnt));
        if (ret < 0)
        {
            /* code */
            printf("%s|%s|%d: setsockopt failed!\n", __FILE__, __func__, __LINE__);
            perror("setsockopt failed");
            break;
        }
        ret = setsockopt(mq_fd->fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof(keepintvl));
        if (ret < 0)
        {
            /* code */
            printf("%s|%s|%d: setsockopt failed!\n", __FILE__, __func__, __LINE__);
            perror("setsockopt failed");
            break;
        }

        printf("%s|%s|%d: Accept a connection from %s:%d\n", __FILE__, __func__, __LINE__, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // 创建接收线程
        pthread_create(&pread_tid, NULL, pread_socket, (void *)mq_fd);
        // 分离接收线程
        pthread_detach(pread_tid);
    }
    free(mq_fd);
    free(cntl_info);

    pthread_exit(0);
}
static void *tcp_socket_set(void *arg)
{
}

struct control tcp_socket_control = {
    .control_name = "tcp_socket",
    .control_init = tcp_socket_init,
    .final = tcp_socket_final,
    .get = tcp_socket_get,
    .set = tcp_socket_set,
    .next = NULL,
};

// 函数：将套接字添加到控制列表中
// 参数：控制列表的头指针
// 返回值：控制列表的头指针
struct control *add_socket_to_cntl_list(struct control *phead)
{
    // 调用add_interface_to_cntl_list函数，将tcp_socket_control添加到控制列表中
    return add_interface_to_cntl_list(phead, &tcp_socket_control);
}
