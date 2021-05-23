#include<stdio.h>
#include "dateTime.h"
int main( void )
{
	char dump[30];
	;
	// 日時構造体を用いて設定
	DateTimeSt stDate;
	stDate.Year = 2021;
	stDate.Month = 5;
	stDate.Day = 22;
	stDate.Hour = 22;
	stDate.Min = 4;
	stDate.Sec = 12;
	DateTime dt1 = DateTime( stDate );

	printf( "%u\n", dt1.ToUnixTime( ) );
	printf( "%s\n", dt1.ToString( dump ) );

	// RTCのレジスタ(BCD表記)を用いて設定
	uint8_t rtcTest[7];
	rtcTest[0] = 0x56;// 秒
	rtcTest[1] = 0x34;// 分
	rtcTest[2] = 0x12;// 時
	rtcTest[3] = 0x00;// 曜日(無視)
	rtcTest[4] = 0x23;// 日
	rtcTest[5] = 0x12;// 月
	rtcTest[6] = 0x53;// 年
	DateTime dt2 = DateTime( rtcTest );
	printf( "%s\n", dt2.ToString( dump ) );

	// GPS(TinyGPS++ライブラリ)の取得値を用いて設定
	uint32_t gpsDate = 231253;//2053/12/23
	uint32_t gpsTime = 14213402;//14:21:13.02
	DateTime dt3 = DateTime( gpsDate, gpsTime );
	printf( "%s\n", dt3.ToString( dump ) );

	// UnixTimeから設定
	uint32_t unixTime = 987654321;// 2001-04-19 04:25:21
	DateTime dt4 = DateTime( unixTime );
	printf( "%u\n", dt4.ToUnixTime( ) );
	printf( "%s\n", dt4.ToString( dump ) );

	
	uint8_t rtc[7];
	dt3.ToRtcData( rtc );

	getchar( );
	return 0;
}