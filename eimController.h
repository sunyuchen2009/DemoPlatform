/************************************************************************************************
*--------------File Info-------------------------------------------------------------------------
* File name:            eim_fpga.h
* Last modified Date:   2022-09-07
* Last Version:
* Descriptions:         eim接口头文件
*--------------------------------------------------------------------------------------------------
* Created by: 		tang
* Created date:         2022-09-07
* Version:              1.0.0
* Descriptions: 	The original version
*
*--------------------------------------------------------------------------------------------------
* Modified by: 		
* Modified date:
* Version:
* Descriptions:
*--------------------------------------------------------------------------------------------------
***************************************************************************************************/

#ifndef __EIM_FPGA_H__
#define __EIM_FPGA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <semaphore.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <signal.h>

#include <stdbool.h>

#define	EIM_FPGA_CNT			1					//设备数量
#define	EIM_FPGA_NAME			"eim_fpga"			//设备文件名
#define	EIM_FPGA_DEV_NAME		"/dev/eim_fpga"		//设备文件名
#define EIM_CS0_PHY_START_ADDR	0x08000000			//EIM CS0的起始地址
#define	EIM_MEM_SIZE			0x4000000			//EIM空间大小,CS0/CS1各占用64M
#define EIM_CS1_PHY_START_ADDR	(EIM_CS0_PHY_START_ADDR + EIM_MEM_SIZE)	//EIM CS1的起始地址
#define	DEFAULT_TX_TIMEOUT		500					//写入默认超时时间
#define	DEFAULT_RX_TIMEOUT		500					//读取默认超时时间
#define DEFAULT_EVENT_TIMEOUT	100					//事件默认超时时间
#define	EIM_RX_BUFF_SIZE		(128 * 1024)		//接收缓存区大小
#define	EIM_IOC_SET_FPGA_PARAM	_IOW('V',23,int)	//设置FPGA参数


/***********************************************************************
*************************IOCTRL命令定义*********************************/
#define	EIM_IOC_SET_READ_OFFSET		_IOW('V',1,int)		//设置读取的偏移地址
#define	EIM_IOC_SET_READ_TIMEOUT	_IOW('V',2,int)		//设置读取超时时间

#define	EIM_IOC_WRITE_FPGA_REG		_IOW('V',3,int)		//写FPGA的寄存器
#define	EIM_IOC_READ_FPGA_REG		_IOWR('V',4,int)	//读FPGA的寄存器

#define	EIM_IOC_SET_WRITE_OFFSET	_IOW('V',5,int)		//设置写入的偏移地址

#define	EIM_IOC_WAIT_EVENT		_IO('V',6)		//等待FPGA事件
#define	EIM_IOC_SET_EVENT_TIMEOUT	_IOW('V',7,int)		//设置事件超时时间

#define	EIM_IOC_GET_DATA		_IOWR('V',8,int)	//读波形数据

#define	EIM_IOC_FPGA_CODE_SIZE		_IOW('V',10,int)	//FPGA代码长度
#define	EIM_IOC_FPGA_CODE		_IOW('V',11,int)	//FPGA代码
#define	EIM_IOC_CONFIG_FPGA		_IOR('V',12,int)	//配置FPGA

#define	EIM_IOC_CLEAR_EVENT		_IO('V',13)		//清除FPGA事件

#define	EIM_IOC_SET_WRITE_TIMEOUT	_IOW('V',14,int)	//设置读取超时时间
#define	EIM_IOC_SET_IMAGE_SIZE		_IOW('V',15,int)	//设置图片的尺寸
#define	EIM_IOC_CAL_BUFF0_RESULT	_IOR('V',16,int)	//计算BUFF0内图片数据的计算结果
#define	EIM_IOC_CAL_BUFF1_RESULT	_IOR('V',17,int)	//计算BUFF1内图片数据的计算结果
#define	EIM_IOC_START_BUFF0_CAL		_IO('V',18)		//启动BUFF0的计算
#define	EIM_IOC_START_BUFF1_CAL		_IO('V',19)		//启动BUFF1的计算
#define	EIM_IOC_GET_BUFF0_RESULT	_IOR('V',20,int)	//获取BUFF0内图片数据的计算结果
#define	EIM_IOC_GET_BUFF1_RESULT	_IOR('V',21,int)	//获取BUFF1内图片数据的计算结果

#define	EIM_IOC_FPGA_RESET		_IO('V',22)		//复位FPGA
/***********************************************************************
*************************FPGA寄存器定义*********************************/
typedef union _FpgaRegDef{
	unsigned int a;
	struct
	{
		unsigned int val	: 16;
		unsigned int offset	: 16;		
	}b;
}FpgaRegDef;
/***********************************************************************
***************************全局函数定义*********************************/
extern	int InitEimPort(unsigned int width,unsigned int height,unsigned int fpga_timeout);
extern	int CalculateImageResult(int fd,unsigned char buff_index,short *result0,short *result1);
extern	int StartCalculateImageResult(int fd,unsigned char buff_index);
extern	int GetImageResult(int fd,unsigned char buff_index,short *result0,short *result1);
extern	short * GetImageBuff(char buff_index);
extern	unsigned int GetImageSize(void);
extern	int ConfigFpga(int fd, char *fpga_fw_path);
extern	int ResetFpga(int fd);
extern	int SetFpgaParams(int fd,unsigned short param0,unsigned short param1);

extern unsigned short ReadFpgaReg(int fd, unsigned short addr);

#endif
