# DemoPlatform
A hardware demonstration platform for an image processing system.

开发环境使用飞凌嵌入式OKMX6DL-C芯片，编译环境为Linux4.1.15 + QT5.6，使用飞凌提供的交叉编译工具。
本项目为某图像处理系统的演示GUI平台，该系统主要包括ARM + FPGA + 上位机三个部分，其中ARM部分包括应用层的GUI代码以及驱动层的FPGA驱动代码。
本项目主要实现了SOCKET通信接收上位机输入的图片数据，以及通过驱动层传输到FPGA进行加速处理，并且可以实时显示测试图片及对应的测试结果，测试结束后也会输出各项统计结果并写入文本文件。
