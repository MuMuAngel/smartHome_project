#include "socket.h"

int socket_init(const char *addr, const char *port)
{
    int s_fd, c_fd;
    int ret = -1;

    // 1.创建套接字
    struct sockaddr_in server_addr;
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd < 0)
    {
        perror("socket creation failed");
        return -1;
    }

    // 2.设置服务器地址和端口号
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(addr); // 也可以使用 inet_aton("127.0.0.1", &server_addr.sin_addr);

    // 3.绑定套接字到指定的地址和端口
    if (bind(s_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind socket failed");
        close(s_fd);
        return -1;
    }

    // 4.监听
    if (listen(s_fd, 10) < 0)
    {
        perror("listen socket failed");
        close(s_fd);
        return -1;
    }

    return s_fd;
    
}