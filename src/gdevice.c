#include "gdevice.h"

#include <stdio.h>
#include <string.h>

#if 0
// 函数：将接口添加到设备列表中
// 参数：phead：设备列表头指针，control_interface：要添加的接口指针
// 返回值：设备列表头指针
struct gdevice *add_interface_to_device_list(struct gdevice *phead, struct gdevice *control_interface)
{
  // 如果设备列表为空，则将接口指针赋值给设备列表头指针
  if (phead == NULL)
  {
    phead = control_interface;
  }
  // 否则，将接口指针的next指针指向设备列表头指针，并将设备列表头指针指向接口指针
  else
  {
    control_interface->next = phead;
    phead = control_interface;
  }
  // 返回设备列表头指针
  return phead;
}
#endif
// 根据key值查找设备
struct gdevice *find_device_by_key(struct gdevice *phead, int key)
{
  // 定义一个指针p，指向头节点
  struct gdevice *p = phead;

  // 如果头节点为空，返回NULL
  if (phead == NULL)
  {
    return NULL;
  }

  // 遍历链表，查找key值
  while (p != NULL)
  {
    // 如果找到key值，返回该节点
    if (p->key == key)
    {
      return p;
    }
    // 否则，指针指向下一个节点
    p = p->next;
  }
  // 如果遍历完整个链表都没有找到key值，返回NULL
  return NULL;
}

struct gdevice *find_device_by_name(struct gdevice *phead, char *name)
{
  // 定义一个指针p，指向头节点
  struct gdevice *p = phead;

  // 如果头节点为空，返回NULL
  if (phead == NULL)
  {
    return NULL;
  }

  // 遍历链表，查找name值
  while (p != NULL)
  {
    // 如果找到name值，返回该节点
    if (strcmp(p->dev_name, name) == 0)
    {
      return p;
    }
    // 否则，指针指向下一个节点
    p = p->next;
  }
  // 如果遍历完整个链表都没有找到name值，返回NULL
  return NULL;
}

int set_gpio_gdevice_status(struct gdevice *pdev)
{
  // 判断pdev是否为空
  if (pdev == NULL)
  {
    return -1;
  }

  // 判断gpio_pin是否为-1
  if (-1 != pdev->gpio_pin)
  {
    // 判断gpio_mode是否为-1
    if (pdev->gpio_mode != -1)
    {
      pinMode(pdev->gpio_pin, pdev->gpio_mode); // 设置GPIO模式
    }

    // 判断gpio_status是否为OUTPUT
    if (pdev->gpio_status != -1)
    {
      digitalWrite(pdev->gpio_pin, pdev->gpio_status); // 设置GPIO状态
    }

    return 0;
  }
}