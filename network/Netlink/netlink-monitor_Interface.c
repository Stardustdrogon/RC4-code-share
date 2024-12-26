#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <time.h>
#include <linux/if.h>

// 日志文件路径
#define LOG_FILE "/tmp/network-status.log"

// 记录日志
void write_log(const char *message) {
    time_t now;
    struct tm *timeinfo;
    char timestamp[64];
    
    // 获取当前时间
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    // 日志
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file != NULL) {
        fprintf(log_file, "%s - %s\n", timestamp, message);
        fclose(log_file);
    }
}

// 发送桌面通知
void send_notification(const char *interface, const char *message) {
    char command[256];
    snprintf(command, sizeof(command), "notify-send 'Network Status' '%s: %s' -u normal -i network-wired", interface, message);
    system(command);

    write_log(message);
}

int main() {
    // 创建 Netlink socket, 用于与内核通信，接收网络接口状态变化的消息
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK; // 监听接口状态

    // 将套接字绑定到指定的地址上
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1; 
    }

    // 接收消息
    char buffer[4096];
    while (1) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len < 0) {
            perror("recv");
            continue;
        }

        // 解析消息
        struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;
        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == RTM_NEWLINK) {
                struct ifinfomsg *ifi = NLMSG_DATA(nlh);
                char ifname[IF_NAMESIZE];
                if_indextoname(ifi->ifi_index, ifname);

                // 判断接口类型
                if (strcmp(ifname, "wlan0") == 0 || strcmp(ifname, "wlp2s0") == 0) { // wifi接口名称
                    if (ifi->ifi_flags & IFF_LOWER_UP) {
                        printf("%s is UP\n", ifname);
                        send_notification(ifname, "Wi-Fi connected");
                    } else {
                        printf("%s is DOWN\n", ifname);
                        send_notification(ifname, "Wi-Fi disconnected");
                    }
                } else if (strcmp(ifname, "eth0") == 0 || strcmp(ifname, "enp0s3") == 0) { // ethernet接口名称
                    if (ifi->ifi_flags & IFF_LOWER_UP) {
                        printf("%s is UP\n", ifname);
                        send_notification(ifname, "Ethernet cable connected");
                    } else {
                        printf("%s is DOWN\n", ifname);
                        send_notification(ifname, "Ethernet cable disconnected");
                    }
                }
            }
        }
    }

    close(sock);
    return 0;
}