#ifndef __GPS_ANALYSE_H__
#define __GPS_ANALYSE_H__

typedef unsigned int UINT;
typedef int BYTE;

typedef struct __gngga__
{
    UINT time;         					/* gps定位时间 */
    UINT latitude;    					/* 纬度整数 */
	UINT latitude_x;    				/* 纬度整数小数 */
    UINT longitude;   					/* 经度整数 */
	UINT longitude_x;   				/* 经度整数小数 */
	UINT GPS_quilty;					/* GPS质量 */
    UINT Number_of_Satellites_in_Use;   /* 卫星数量 */
	UINT HDOP;							/* HDOP */
	UINT HDOP_x;						/* HDOP小数 */
    UINT Altitude; 						/* 海拔 */
	UINT Altitude_x; 					/* 海拔小数 */
    UINT Geoidal_Separation; 		    /* 大地水准位间距 */
	UINT Geoidal_Separation_x; 		    /* 大地水准位间距 */
	char str;
} GNGGA;

extern int set_option(int fd, int nSpeed, int nBits, char nEvent, int nStop);
extern int print_gps(GNGGA *gps_date);
extern int gps_analyse(char *buff, GNGGA *gps_data);
#endif