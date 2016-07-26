2016.07.26
NPLink-Mote-SDK-0.0.8.20160726_release
------------------------------------
1、增加并优化通过API进行多个channel的设置

2016.07.10
NPLink-Mote-SDK-0.0.7.20160710_release
------------------------------------
1、增加支持490频段数据包的收发
2、请使用编译器Keil uVision 5.16a打开本工程

2016.06.28
NPLink-Mote-SDK-0.0.6.20160628_release
------------------------------------
1、增加支持433、470、868、915频段数据包的收发
2、请使用编译器Keil uVision 5.16a打开本工程

2016.05.31
NPLink-Mote-SDK-0.0.4.20160531_release
------------------------------------
1、增加OLED屏按行清空函数
2、增加支持发送功率及FSK相关参数设置
3、请使用编译器Keil uVision 5.16a打开本工程

2016.04.13
NPLink-Mote-SDK-0.0.4.20160412_release
------------------------------------
1、删除Mote ID 的设置

2016.03.22
NPLink-Mote-SDK-SRC-0.0.4.20160308_Beta
------------------------------------
1、添加支持低功耗（USE_LOW_POWER_MODE编译器宏定义），在没有外部IO设备（如LED，按键，UART等）的情况下，休眠电流约为12uA
2、添加支持phy MAC（即不带协议的数据收发），且phy MAC工作模式下，调制方式可选择为LORA或FSK
3、增加一些参数设置
4、添加支持USE_DEBUG编译器宏定义，来开关串口调试信息
5、

2016.03.08
NPLink-Mote-SDK-SRC-0.0.4.20160308_Beta
------------------------------------
1.添加支持PHY模式下Mote到Mote的点对点通讯，可通过API设置PHY模式下的通信参数、在LoRaMac与PHY模式之间进行切换。
2.添加radio部分的低功耗API，对sx芯片进行休眠控制及关闭/开启sx芯片的外部有源晶振。

2015.11.04
NPLink-Mote-SDK-0.0.1.20151104_beta
------------------------------------
1. 创建SDK开发者文档。
2.将工程中的node修改为mote。

2015.11.02
NPLink-Mote-SDK-0.0.1.20151102_beta
------------------------------------
1. 增加参数获取及参数设定API函数：LoRaMac_getParameter(u8* pBuffer, u8 parameterID)、LoRaMac_setParameter(u8* pBuffer, u8 parameterID)
2. 支持moteID号获取。
3. 将moteID号封装到LoRaMac lib中。

------------------------------------
2015.10.22
1、生成发布版本代码，实现LoRaMac classA, OSAL等功能