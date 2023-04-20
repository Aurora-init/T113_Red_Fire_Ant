#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "./gps_analyse.h"

char str[100];

int gps_analyse(char *buff, GNGGA *gps_data)
{
    char *ptr = NULL;
    
    if (gps_data == NULL)
    {
        return -1;
    }
    if (strlen(buff) < 10)
    {
        return -1;
    }
    /* 如果buff字符串中包含字符"$GPRMC"则将$GPRMC的地址赋值给ptr */
    if (NULL == (ptr = strstr(buff, "$GNGGA")))
    {
        return -1;
    }

    /* sscanf函数为从字符串输入，意思是将ptr内存单元的值作为输入分别输入到后面的结构体成员 */
			  // $GNGGA,110823.000,2311.77676,N,11323.44831,E,1,22,0.8,45.6,M,0.0,M,,*4A 
    sscanf(ptr, "$GNGGA,%d.000,%d.%d,N,%d.%d,E,%d,%d,%d.%d,%d.%d,M,%d.%d,M,,*4%c",
           &(gps_data->time),                			/* gps定位时间 */
           &(gps_data->latitude),            			/* 纬度 */
           &(gps_data->latitude_x),            			/* 纬度小数 */
           &(gps_data->longitude),           			/* 经度 */
           &(gps_data->longitude_x),           			/* 经度小数 */
           &(gps_data->GPS_quilty), 		 			/* GPS质量 */
           &(gps_data->Number_of_Satellites_in_Use),    /* 卫星数量 */
           &(gps_data->HDOP),							/* HDOP */
           &(gps_data->HDOP_x),							/* HDOP小数 */
           &(gps_data->Altitude),	 			        /* 海拔 */
           &(gps_data->Altitude_x),	 			        /* 海拔小数 */
           &(gps_data->Geoidal_Separation), 		    /* 大地水准位间距 */
           &(gps_data->Geoidal_Separation_x), 		    /* 大地水准位间距 */
           &(gps_data->str)); 		    				/* 校验 */
    
    sscanf(ptr,"%s",str);
    return 0;
}

int print_gps(GNGGA *gps_data)
{
	printf("============================================================\n");
	printf("===                                                      ===\n");
	printf("==%s\n",str);
	printf("==时间 : %02d:%02d:%02d\n", (gps_data->time / 10000 + 8) % 24, (gps_data->time % 10000) / 100, gps_data->time % 100);
	printf("==维度 : 北纬%02d度%7.5f分\n", ((int)gps_data->latitude)  / 100, (float)((gps_data->latitude)-(gps_data->latitude)/100*100+(float)(gps_data->latitude_x)/100000));
    printf("==经度 : 东经%02d度%7.5f分\n", ((int)gps_data->longitude)  / 100, (float)((gps_data->longitude)-(gps_data->longitude)/100*100+(float)(gps_data->longitude_x)/100000));
	printf("==使用的卫星数量 : %d个\n", gps_data->Number_of_Satellites_in_Use);
	printf("==HDOP : %d.%d\n", gps_data->HDOP,gps_data->HDOP_x);
	printf("==海拔 : %d.%d\n", gps_data->Altitude,gps_data->Altitude_x);
	printf("==大地水准位间距 : %d.%d\n", gps_data->Geoidal_Separation,gps_data->Geoidal_Separation_x);
	printf("===                                                      ===\n");
	printf("============================================================\n");
    return 0;
}
