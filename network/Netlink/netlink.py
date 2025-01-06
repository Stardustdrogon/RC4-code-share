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
        self.ip = IPRoute() # 创建IPRoute对象，创建套接字与内核通信
        self.selector = selectors.DefaultSelector() # 创建selector对象，监视文件描述符
        self.monitored_interfaces = ['wlan0', 'enp3s0'] # 监控的接口列表
        self.last_carrier = {} # 存储每个接口的上一次状态
        
        # 初始化接口状态
        self.init_interface_states()
        
    def get_carrier_state(self, ifname):
        # 从sysfs获取接口的carrier状态(设置last_carrier的初始化状态)
        try:
            with open(f"/sys/class/net/{ifname}/carrier") as f:
                return int(f.read().strip())
        except:
            return 0
        
    def init_interface_states(self):
        # 初始化所有监控接口的状态
        for ifname in self.monitored_interfaces:
            carrier = self.get_carrier_state(ifname)
            self.last_carrier[ifname] = carrier
        
    def handle_event(self, fileobj, mask):
        # 处理网络接口事件
        for msg in self.ip.get():
            if msg['event'] == 'RTM_NEWLINK': # 接口状态变化事件
                ifname = msg.get_attr('IFLA_IFNAME')
                if ifname in self.monitored_interfaces:
                    carrier = msg.get_attr('IFLA_CARRIER') # 获取carrier状态
                    if carrier is not None:
                        if self.last_carrier.get(ifname) != carrier: # 只在carrier状态变化时记录
                            status = "connected" if carrier == 1 else "disconnected"
                            msg = f"Interface: {ifname} - Status: {status}"
                            logging.info(msg)
                            print(msg)
                            self.last_carrier[ifname] = carrier

    def run(self):
        # 开始监控网络接口状态
        try:
            logging.info("Network monitoring started")
            self.ip.bind(groups=RTMGRP_LINK)
            
            # 注册套接字到selector 
            self.selector.register(
                self.ip.fileno(),
                selectors.EVENT_READ,
                self.handle_event
            )
            
            while True:
                events = self.selector.select()
                for key, mask in events: 
                    callback = key.data  # 获取回调函数
                    callback(key.fileobj, mask) # 调用回调函数
                    
        except Exception as e:
            logging.error(f"Error in monitor: {e}")
        finally:
            self.cleanup()
            
    def cleanup(self):
        self.selector.close()
        self.ip.close()
        logging.info("Network monitoring stopped")

if __name__ == '__main__':
    try:
        monitor = NetworkMonitor()
        monitor.run()
    except KeyboardInterrupt:
        pass