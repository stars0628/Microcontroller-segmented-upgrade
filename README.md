# Microcontroller-segmented-upgrade
 
# 单片机bin分段升级


本仓库包含以下内容：
Segmented_IAP源文件
demo文件
Qt上位机源码

## 仓库说明
1. 对单片机进行IAP升级，面对rom空间不够的单片机，采用将bin文件分段传输写入flash的方式是一个不错的方案

## 使用说明

1. 具体说明见demo中的readme，该demo使用的是国民技术的N32G430芯片。
2. Qt上位机编译后使用串口连接移植好IAP的单片机，选择升级文件（.bin）和保存路径（被分段的bin文件输出保存路径）后，点击升级则可以对单片机进行升级
3. 该项目仅是一个可用的方式，只为了实现升级功能

## Qt信息

![image](https://user-images.githubusercontent.com/43928335/236725046-991ee450-f789-48d8-8176-8a20214d6338.png)

## Qt软件使用
1. 在单片机进入Bootload程序后，依照下图操作，软件上的进入升级模式按钮需要在APP中做了接收到升级命令后跳转到Bootload程序才可用，测试时可以让Bootload程序直接处于需要升级的状态
![image](https://github.com/stars0628/Microcontroller-segmented-upgrade/assets/43928335/76f37857-1690-46ac-a747-36037d1c96f9)

2. 在下图中，注释掉标志判断，取消注释"if(0) {"，可以让程序复位后一直运行在iap
![image](https://user-images.githubusercontent.com/43928335/236725930-0ac2061e-73c6-46f4-88b8-f86f14a2ccdc.png)

3. 经测试，是可以升级成功的
![image](https://user-images.githubusercontent.com/43928335/236725551-cc102276-2946-4738-a4a0-f4ebb39df13d.png)

