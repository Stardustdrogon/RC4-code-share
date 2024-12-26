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

#define LOG_FILE "/tmp/network-status.log"

// 查看 carrier 状态
int get_interface_state(const char *ifname) {
    char path[256];
    char carrier;
    
    snprintf(path, sizeof(path), "/sys/class/net/%s/carrier", ifname);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return 0;
    }
    
    carrier = fgetc(f);
    fclose(f);
    return (carrier == '1');
}

void write_log(const char *message) {
    time_t now;
    struct tm *timeinfo;
    char timestamp[64];
    
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file != NULL) {
        fprintf(log_file, "%s - %s\n", timestamp, message);
        fclose(log_file);
    }
}

void send_notification(const char *interface, const char *status) {
    char message[256];
    snprintf(message, sizeof(message), "Interface: %s Status: %s", interface, status);
    write_log(message);
}

int main() {
    // 初始化状态
    int last_wlan_state = get_interface_state("wlan0");
    int last_eth_state = get_interface_state("enp3s0");

    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK;  // 只关注链路状态变化

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1; 
    }

    char buffer[4096];
    while (1) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len < 0) {
            perror("recv");
            continue;
        }

        struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;
        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == RTM_NEWLINK) {
                struct ifinfomsg *ifi = NLMSG_DATA(nlh);
                char ifname[IF_NAMESIZE];
                if_indextoname(ifi->ifi_index, ifname);

                // 获取当前 carrier 状态
                int current_state = get_interface_state(ifname);

                // WiFi 接口
                if (strcmp(ifname, "wlan0") == 0) {
                    if (current_state && !last_wlan_state) {
                        printf("%s is connected\n", ifname);
                        send_notification(ifname, "connected");
                        last_wlan_state = 1;
                    } else if (!current_state && last_wlan_state) {
                        printf("%s is disconnected\n", ifname);
                        send_notification(ifname, "disconnected");
                        last_wlan_state = 0;
                    }
                }
                // 以太网接口
                else if (strcmp(ifname, "enp3s0") == 0) {
                    if (current_state && !last_eth_state) {
                        printf("%s is connected\n", ifname);
                        send_notification(ifname, "connected");
                        last_eth_state = 1;
                    } else if (!current_state && last_eth_state) {
                        printf("%s is disconnected\n", ifname);
                        send_notification(ifname, "disconnected");
                        last_eth_state = 0;
                    }
                }
            }
        }
    }

    close(sock);
    return 0;
}