#ifndef __CONTROL_H__
#define __CONTROL_H__

struct control
{
    /* data */
    char control_name[128];     //控制模块名称
    int (*control_init)(void);  //控制模块初始化函数
    void (*final)(void);        //控制模块销毁函数
    void *(*get)(void *arg);      //监听函数
    void *(*set)(void *arg);      //设置函数

    struct control *next;
};

struct control *add_interface_to_cntl_list(struct control *phead, struct control *pnew);

#endif