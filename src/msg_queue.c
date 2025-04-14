#include "msg_queue.h"

// 创建消息队列
mqd_t msg_queue_init(void)
{
    // 定义消息队列属性
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSG_NUM; // 消息队列中最大消息数
    attr.mq_msgsize = MAX_MSG_SIZE; // 每条消息的最大字节数
    attr.mq_flags = 0; // 消息队列标志

    // 打开消息队列
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1)
    {
        perror("mq_open"); // 打印错误信息
        return -1;
    }

    printf("%s|%s|%d: mqd = %d\n", __FILE__, __func__, __LINE__, mq); // for debug
    return mq;
}

// 函数send_msg用于向指定的消息队列mq发送消息msg，消息长度为msg_len
int msg_queue_send(mqd_t mq, void *msg, size_t msg_len)
{
    // 调用mq_send函数发送消息，返回发送的字节数
    int send_byte = mq_send(mq, (char *)msg, msg_len, 0);
    // 如果发送失败，输出错误信息并返回-1
    if (send_byte == -1)
    {
        perror("mq_send");
        return -1;
    }
    // 如果发送成功，返回发送的字节数
    return send_byte;
}

void msg_queue_destroy(mqd_t mq)
{
    // 调用mq_close函数关闭消息队列
    mq_close(mq);
    // 调用mq_unlink函数删除消息队列
    mq_unlink(QUEUE_NAME);
}