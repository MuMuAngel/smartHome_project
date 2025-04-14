#if 0
1、包含Python.h头文件，以便使用Python API。
2、使用void Py_Initialize()初始化Python解释器，
3、使用PyObject *PyImport_ImportModule(const char *name)和PyObject *PyObject_GetAttrString(PyObject *o, const char *attr_name)获取sys.path对象，并利用int PyList_Append(PyObject *list, PyObject *item)将当前路径.添加到sys.path中，以便加载当前的Python模块(Python文件即python模块)。
4、使用PyObject *PyImport_ImportModule(const char *name)函数导入Python模块，并检查是否有错误。
5、使用PyObject *PyObject_GetAttrString(PyObject *o, const char *attr_name)函数获取Python函数对象，并检查是否可调用。
6、使用PyObject *Py_BuildValue(const char *format, ...)函数创建一个Python元组，作为Python函数的参数，没有参数时不需要调用
7、使用PyObject *PyObject_CallObject(PyObject *callable, PyObject *args)函数调用Python函数，并获取返回值。
8、使用int PyArg_Parse(PyObject *args, const char *format, ...)函数将返回值转换为C类型，并检查是否有错误,没有返回值时不需要调用。
9、使用void Py_DECREF(PyObject *o)函数释放所有引用的Python对象。
10、结束时调用void Py_Finalize()函数关闭Python解释器。
相关的函数参数说明参考网站（网站左上角输入函数名即可开始搜索）：
https://docs.python.org/zh-cn/3/c-api/import.html
#endif

#include <Python.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "face.h"

void face_init(void)
{
    Py_Initialize(); // 初始化Python解释器
    if (!Py_IsInitialized())
    {
        fprintf(stderr, "初始化Python解释器失败\n");
        goto error;
    }
    PyObject *sys = PyImport_ImportModule("sys");
    PyObject *path = PyObject_GetAttrString(sys, "path");
    PyObject *current_dir = PyUnicode_FromString(".");
    if (PyList_Append(path, current_dir) == -1)
    {
        PyErr_Print();
        Py_DECREF(current_dir);
    }
error:
    Py_DECREF(sys);
    Py_DECREF(path);
    Py_DECREF(current_dir);
}

void face_final(void)
{
    Py_Finalize();
}

double face_data(void)
{
    double result = 0.00;
    system(WGET_CMD);
    if (0 != access(FACE_FILE, F_OK))
    {
        /* code */
        return result;
    }

    // 1.导入Python模块
    PyObject *pModule = PyImport_ImportModule("face");
    if (!pModule)
    {
        PyErr_Print();
        fprintf(stderr, "Error: failed to load face.py\n");
        goto error;
    }
    // 2.获取函数对象
    PyObject *pFunc = PyObject_GetAttrString(pModule, "aliyun_face");
    if (!pFunc)
    {
        PyErr_Print();
        fprintf(stderr, "Error: failed to load aliyun_face\n");
        goto error;
    }
    // 3.构建参数（以传递整数42为例）本次调用函数不需要传递参数
    // PyObject *pArgs = Py_BuildValue("(i)", 42);  // 格式字符串"(i)"表示整数参数

    // 4.调用函数
    PyObject *pResult = PyObject_CallObject(pFunc, NULL);
    if (!pResult)
    {
        PyErr_Print();
        fprintf(stderr, "Error: Function call failed\n");
        goto error;
    }

    // 5.解析返回值

    if (!PyArg_Parse(pResult, "d", &result))
    {
        PyErr_Print();
        goto error;
    }

error:
    Py_XDECREF(pModule);
    Py_XDECREF(pFunc);
    Py_XDECREF(pResult);

    return result;
}

// int main(int argc, char *argv[])
// {
//     double x;
//     face_init();
//     x = face_data();
//     face_final();

//     printf("x = %lf\n", x);

//     return 0;
// }
