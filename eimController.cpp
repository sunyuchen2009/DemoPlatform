/************************************************************************************************
*--------------File Info-------------------------------------------------------------------------
* File name:            eim_fpga.c
* Last modified Date:   2022-09-07
* Last Version:
* Descriptions:         eim接口源文件
*--------------------------------------------------------------------------------------------------
* Created by:       tang
* Created date:         2022-09-07
* Version:              1.0.0
* Descriptions:     The original version
*
*--------------------------------------------------------------------------------------------------
* Modified by:      
* Modified date:
* Version:
* Descriptions:
*--------------------------------------------------------------------------------------------------
***************************************************************************************************/

#include "eimController.h"

static short *image_data[2] = {NULL,NULL};          //图片数据缓存区
static unsigned int image_size = 0;                 //图片尺寸
/***************************************************************************************
* Function name         函数名称:    InitEimPort
* Description           功能描述:    初始化Eim端口
* Input parameters      输入参数:    width:图片宽度，height:图片高度
* Returned value        返回参数:    Eim设备文件描述符，若<=0，代表设备初始化失败
* Used global variables 全局变量:    初始化成功后，全局变量image_data保存两幅图片的首地址，
                     image_size保存图片尺寸
* Calling funcations    调用函数:    无
* 
* Created by     作　者:    tang
* Created Date   日　期:    2022.09.05
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int InitEimPort(unsigned int width,unsigned int height,unsigned int fpga_timeout)
{
    int fd = open(EIM_FPGA_DEV_NAME,O_RDWR);
    if(fd <= 0)
    {
        printf("Open device file failed!\r\n");
    }
    else
    {
        image_size = width * height;
        if( (image_size % 2) != 0 )                                 //如果给定的图像尺寸为奇数
        {
            image_size = (image_size / 2 + 1) * 2;                          //因为EIM的DMA操作需4字节对齐，故将图片尺寸加一
        }

        if(ioctl(fd, EIM_IOC_SET_IMAGE_SIZE, &image_size) != 0)
        {
            printf("Set image size error!\n");
            close(fd);
            return -1;
        }
        printf("image_size = %d.\n", image_size);
            
        if(ioctl(fd, EIM_IOC_SET_EVENT_TIMEOUT, &fpga_timeout) != 0)                    //等待FPGA DOK拉高时间超时时间，单位ms
        {
            printf("Set event timeout error!\n");
            close(fd);
            return -1;
        }
        
        short *ptr = (short *)mmap(NULL, image_size * 4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);  //以图片尺寸2倍大小映射缓存区
        if (ptr == MAP_FAILED)
        {
            printf("mmap failed\n");
            close(fd);
            return -1;
        }
        image_data[0] = ptr;
        image_data[1] = ptr + image_size;
    }
    return fd;
}

/***************************************************************************************
* Function name         函数名称:    CalculateImageResult
* Description           功能描述:    将缓存区buff_index中的数据写入到FPGA，
                     从FPGA获取图片特征值，函数将阻塞到计算完成
* Input parameters      输入参数:    fd:Eim设备文件描述符
                     buff_index:图片缓存区索引（0或1）
                     result0:计算结果1
                     result1:计算结果2
* Returned value        返回参数:    处理结果，0:正确得到计算结果，非0:错误代码
                     -1：启动计算前DOVER超时未拉高
                     -2：通过EIM总线写图片数据到FPGA出错
                     -3：写数据结束超时未等到OVER和DOK拉高
                     -4：写数据结束后超时未等到DOK未拉高，OVER拉高，（当前图片为空）
                     -10：图片缓存区参数错误（只能0或1）
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    无
* 
* Created by     作　者:    tang
* Created Date   日　期:    2022.09.06
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int CalculateImageResult(int fd,unsigned char buff_index,short *result0,short *result1)
{
    unsigned int result = 0;
    int pro_res = 0;
    switch(buff_index)
    {
        case 0:
        {
            pro_res = ioctl(fd, EIM_IOC_CAL_BUFF0_RESULT, &result);
            break;
        }
        case 1:
        {
            pro_res = ioctl(fd, EIM_IOC_CAL_BUFF1_RESULT, &result);
            break;
        }
        default:
        {
            pro_res = -10;
            break;
        }
    }

    if(pro_res == 0)
    {
        if(result0 != NULL)
        {
            *result0 = (short)(result & 0xFFFF);
        }
        if(result1 != NULL)
        {
            *result1 = (short)(result >> 16);
        }
    }

    if(pro_res == 0 || pro_res == -10)
    {
        return pro_res;
    }
    else
    {
        return -errno;
    }
    
    
}

/***************************************************************************************
* Function name         函数名称:    StartCalculateImageResult
* Description           功能描述:    启动缓存区的图片特征值计算，函数启动计算后立即返回
* Input parameters      输入参数:    fd:Eim设备文件描述符
                     buff_index:图片缓存区索引（0或1）
* Returned value        返回参数:    处理结果，0:正常启动，非0:错误代码
                     -10：图片缓存区参数错误（只能0或1）
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    无
* 
* Created by     作　者:    tang
* Created Date   日　期:    2022.09.06
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int StartCalculateImageResult(int fd,unsigned char buff_index)
{
    int pro_res = 0;
    switch(buff_index)
    {
        case 0:
        {
            pro_res = ioctl(fd, EIM_IOC_START_BUFF0_CAL, NULL);
            break;
        }
        case 1:
        {
            pro_res = ioctl(fd, EIM_IOC_START_BUFF1_CAL, NULL);
            break;
        }
        default:
        {
            pro_res = -10;
            break;
        }
    }

    return pro_res; 
}

/***************************************************************************************
* Function name         函数名称:    GetImageResult
* Description           功能描述:    获取缓存区的图片计算的特征值，若计算未结束将阻塞到计算结束
* Input parameters      输入参数:    fd:Eim设备文件描述符
                     buff_index:图片缓存区索引（0或1）
                     result0:计算结果1
                     result1:计算结果2
* Returned value        返回参数:    处理结果，0:正确得到计算结果，非0:错误代码
                     -1：启动计算前DOVER超时未拉高
                     -2：通过EIM总线写图片数据到FPGA出错
                     -3：写数据结束超时未等到OVER和DOK拉高
                     -4：写数据结束后超时未等到DOK未拉高，OVER拉高，（当前图片为空）
                     -10：图片缓存区参数错误（只能0或1）
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    无
* 
* Created by     作　者:    tang
* Created Date   日　期:    2022.09.06
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int GetImageResult(int fd,unsigned char buff_index,short *result0,short *result1)
{
    unsigned int result = 0;
    int pro_res = 0;
    switch(buff_index)
    {
        case 0:
        {
            pro_res = ioctl(fd, EIM_IOC_GET_BUFF0_RESULT, &result);
            break;
        }
        case 1:
        {
            pro_res = ioctl(fd, EIM_IOC_GET_BUFF1_RESULT, &result);
            break;
        }
        default:
        {
            pro_res = -10;
            break;
        }
    }

    if(pro_res == 0)
    {
        if(result0 != NULL)
        {
            *result0 = (short)(result & 0xFFFF);
        }
        if(result1 != NULL)
        {
            *result1 = (short)(result >> 16);
        }
    }

    if(pro_res == 0 || pro_res == -10)
    {
        return pro_res;
    }
    else
    {
        return -errno;
    }
    
    
}

/***************************************************************************************
* Function name         函数名称:    GetImageBuff
* Description           功能描述:    获取图片缓存区首地址
* Input parameters      输入参数:    buff_index:图片缓存区索引（0或1）
* Returned value        返回参数:    图片缓存区到首地址，若缓存区索引错误则返回NULL
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    无
* 
* Created by     作　者:    tang
* Created Date   日　期:    2022.09.07
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
short * GetImageBuff(char buff_index)
{
    if(buff_index < 2)
    {
        return image_data[buff_index];
    }

    return NULL;
}

/***************************************************************************************
* Function name         函数名称:    GetImageSize
* Description           功能描述:    获取图片尺寸
* Input parameters      输入参数:    无
* Returned value        返回参数:    图片尺寸
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    无
* 
* Created by     作　者:    tang
* Created Date   日　期:    2022.09.07
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
unsigned int GetImageSize(void)
{
    return  image_size;
}

/***************************************************************************************
* Function name         函数名称:    ConfigFpga
* Description           功能描述:    配置FPGA
* Input parameters      输入参数:    fd:Eim设备文件描述符
                     fpga_fw_path:FPGA代码路径
* Returned value        返回参数:    无
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    0:配置成功，非0:错误代码
* 
* Created by     作　者:    tang
* Created Date   日　期:    2021.10.27
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int ConfigFpga(int fd, char *fpga_fw_path)
{
    int ret = 0;
    int size = 0;
    struct stat statbuf;
    stat(fpga_fw_path, &statbuf);
    size = statbuf.st_size;
    printf("FPGA fw size == %d.\n",size);
    if(size > 0)
    {
        FILE *fp = fopen(fpga_fw_path, "r");
        if(fp != NULL)
        {
            unsigned char *fpgaFw = (unsigned char *)malloc(size);
            if(fpgaFw != NULL)
            {
                fread(fpgaFw, size, 1 , fp);                //读取FPGA固件内容
                if(ioctl(fd, EIM_IOC_FPGA_CODE_SIZE, &size) == 0)   //FPGA固件大小
                {
                    if(ioctl(fd, EIM_IOC_FPGA_CODE, fpgaFw) == 0)   //FPGA固件内容
                    {
                        int i = 0,configResult = 0;
                        ret = -6;
                        while(i < 3)
                        {
                            if(ioctl(fd, EIM_IOC_CONFIG_FPGA, &configResult) == 0)  //开始配置
                            {
                                if(configResult == 0)
                                {
                                    printf("Config FPGA successfully at %d try.\n", i + 1);
                                    ret = 0;
                                    break;
                                }
                            }
                            printf("Config FPGA failue at %d try.\n", i + 1);
                            i++;
                        }
                        
                    }
                    else
                    {
                        printf("Send FPGA fw to kernal failue!\n");
                        ret = -5;
                    }
                }
                else
                {
                    printf("Set FPGA fw size failue!\n");
                    ret = -4;
                }
                free(fpgaFw);
            }
            else
            {
                ret = -3;
            }
            fclose(fp);
        }
        else
        {
            printf("Open FPGA fw failue!\n");
            ret = -2;
        }
    }
    else
    {
        ret = -1;
    }

    return ret;
}

/***************************************************************************************
* Function name         函数名称:    ResetFpga
* Description           功能描述:    复位FPGA
* Input parameters      输入参数:    fd:Eim设备文件描述符
* Returned value        返回参数:    无
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    0:复位成功，非0:错误代码
* 
* Created by     作　者:    tang
* Created Date   日　期:    2021.10.04
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int ResetFpga(int fd)
{
    return ioctl(fd, EIM_IOC_FPGA_RESET, NULL);
}

/***************************************************************************************
* Function name         函数名称:    SetFpgaParams
* Description           功能描述:    设置FPGA参数，可以不调用
* Input parameters      输入参数:    fd:     Eim设备文件描述符
*                                   param0: 子图个数，FPGA内部默认为5
*                                   param1: 距离，FPGA内部默认为30
* Returned value        返回参数:    无
* Used global variables 全局变量:    无
* Calling funcations    调用函数:    0:复位成功，非0:错误代码
* 
* Created by     作　者:    tang
* Created Date   日　期:    2021.10.04
* --------------------------------------------------------------------------------------
* Modified by                 修改者: 
* Modified date               日　期:
* Modification information  修改信息:
* --------------------------------------------------------------------------------------
***************************************************************************************/
int SetFpgaParams(int fd,unsigned short param0,unsigned short param1)
{
    unsigned short paramsBuff[16] = {0,};
    paramsBuff[0] = param0;
    paramsBuff[1] = param1;
    return ioctl(fd, EIM_IOC_SET_FPGA_PARAM, paramsBuff);
}