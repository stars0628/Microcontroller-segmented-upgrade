# Microcontroller-segmented-upgrade
 
# 单片机bin分段升级


本仓库包含以下内容：
Segmented_IAP源文件
demo文件
Qt上位机源码

## 仓库说明
1. 对单片机进行IAP升级，面对rom空间不够的单片机，采用将bin文件分段传输写入flash的方式是一个不错的方案

## 使用说明

1. 具体说明见demo中的readme
2. Qt上位机编译后使用串口连接移植好IAP的单片机，选择升级文件（.bin）和保存路径（被分段的bin文件输出保存路径）后，点击升级则可以对单片机进行升级
3. 该项目仅是一个可用的方式，只为了实现升级功能