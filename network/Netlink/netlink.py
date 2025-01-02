#!/usr/bin/env python3


import selectors
from pyroute2 import IPRoute
from pyroute2.netlink.rtnl import RTMGRP_LINK
import logging

# 配置日志
LOG_FILE = "/tmp/py.log"
logging.basicConfig(
    filename=LOG_FILE,
    level=logging.INFO,
    format='%(asctime)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

class NetworkMonitor:
    def __init__(self):
        self.ip = IPRoute() # 创建IPRoute对象，创建一个 netlink 套接字
        self.selector = selectors.DefaultSelector() # 创建一个selector对象，用于监视套接字的事件
        self.monitored_interfaces = ['wlan0', 'enp3s0']
        
    def handle_event(self, fileobj, mask):
        # 处理网络接口事件
        for msg in self.ip.get():
            if msg['event'] == 'RTM_NEWLINK':
                ifname = msg.get_attr('IFLA_IFNAME')
                if ifname in self.monitored_interfaces:
                    carrier = msg.get_attr('IFLA_CARRIER')
                    if carrier is not None:
                        status = "connected" if carrier == 1 else "disconnected"
                        logging.info(f"Interface: {ifname} - Status: {status}")
                        print(f"Interface: {ifname} - Status: {status}")

    def run(self):
        #开始监控网络接口状态
        try:
            logging.info("Network monitoring started")
            
            # 绑定netlink socket
            self.ip.bind(groups=RTMGRP_LINK) # 绑定到RTMGRP_LINK组
            
            # 注册事件处理
            self.selector.register(
                self.ip.fileno(),
                selectors.EVENT_READ,
                self.handle_event
            )
            
            # 主循环
            while True:
                events = self.selector.select() # 阻塞等待事件
                for key, mask in events:
                    callback = key.data
                    callback(key.fileobj, mask) 
                    
        except Exception as e:
            logging.error(f"Error in monitor: {e}")
        finally:
            self.cleanup()
            
    def cleanup(self):
        # 清理资源
        self.selector.close()
        self.ip.close()
        logging.info("Network monitoring stopped")

if __name__ == '__main__':
    try:
        monitor = NetworkMonitor()
        monitor.run()
    except KeyboardInterrupt:
        pass
