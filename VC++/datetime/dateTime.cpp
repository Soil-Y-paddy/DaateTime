#include "dateTime.h"

#define UNIX_EPOCH (62135683200)
#define EPOCH_DAY UNIX_EPOCH

//閏年を考慮した年の経過日
#define YEAR_DATE( p_byYear ) (  365 * ( p_byYear ) + (int)( ( p_byYear ) / 4 ) - (int)( ( p_byYear ) / 100 ) + (int)( ( p_byYear ) / 400 ) )
//3月１日からの月の経過日
#define  MonthDate( month ) ((int)( 306 * ( ( month ) + 1 ) / 10 ) )

#define DAY_HOUR (24)
#define HOUR_MIN (60)
#define MIN_SEC (60)
#define SEC_HOUR (3600) // 60*60
#define DAY_MAX	(86400) // 24*60*60
#define BCD2BYTE( data ) ( ( ( (data) & 0xF0 ) >>4 ) * 10 + ( (data) & 0x0F ) )
#define BYTE2BCD( data ) ( ( ( (data) / 10 ) << 4 ) | ( ( data ) % 10 ) )


// 日時構造体から、0/1/1からの経過秒を求める
uint64_t DateTime::CalcDate( DateTimeSt p_stDateSt)
{
	// 1, 2月は前年の13, 14月
	if( p_stDateSt.Month < 3 )
	{
		p_stDateSt.Month+=12;
		p_stDateSt.Year--;
	}
	uint64_t qwDate = (
		YEAR_DATE( p_stDateSt.Year )
		+ MonthDate( p_stDateSt.Month )
		- 428
		+ p_stDateSt.Day 
	);
	qwDate = qwDate * DAY_HOUR + p_stDateSt.Hour;
	qwDate = qwDate * HOUR_MIN + p_stDateSt.Min;
	qwDate = qwDate * MIN_SEC + p_stDateSt.Sec;
	return qwDate;
}

void DateTime::FromStructure( DateTimeSt p_stDate )
{
	Date = CalcDate( p_stDate );

}

DateTime::DateTime( const DateTime &p_Src )
{
	Date = p_Src.Date;
}

// 日時構造体から、DateTimeクラスを生成する
DateTime::DateTime( DateTimeSt p_stDate )
{
	FromStructure( p_stDate );
}


// GPS時間から、DateTimeクラスを生成する
// gpsDate : DDMMYY(dec)
// gpsTime : HHMMSSCC(dec)
DateTime::DateTime( uint32_t gpsDate, uint32_t gpsTime )
{
	DateTimeSt stTemp;
	// gpsDate = DDMMYY (dec)
	// gpsTime = HHMMSSCC (dec)
	stTemp.Year = ( uint16_t ) ( ( gpsDate % 100 ) + 2000 );
	stTemp.Month = ( uint8_t ) ( ( gpsDate / 100 ) % 100 );
	stTemp.Day = ( uint8_t ) ( ( gpsDate / 10000 ) );
	stTemp.Hour = ( uint8_t ) ( gpsTime / 1000000 );
	stTemp.Min = ( uint8_t ) ( ( gpsTime / 10000 ) % 100 );
	stTemp.Sec = ( uint8_t ) ( ( gpsTime / 100 ) % 100 );
	FromStructure( stTemp );
}


// Unix時間からDateTimeクラスを生成する
DateTime::DateTime( uint32_t unixTime )
{
	Date = unixTime + UNIX_EPOCH ;
}

// RTCレジスタからDateTimeクラスを生成する
// サポートチップ：DS3231, RTC-4543SA 
DateTime::DateTime( uint8_t * pRtcDate )
{	
	DateTimeSt stTemp;
	stTemp.Sec = BCD2BYTE( pRtcDate[0] );
	stTemp.Min = BCD2BYTE( pRtcDate[1] );
	stTemp.Hour = BCD2BYTE( pRtcDate[2] );
	stTemp.Day = BCD2BYTE( pRtcDate[4] );
	stTemp.Month = BCD2BYTE( pRtcDate[5] );
	stTemp.Year = BCD2BYTE( pRtcDate[6] ) + 2000;
	FromStructure( stTemp );

}

char * DateTime::ToString( char * buf )
{
	DateTimeSt sp = ToStructur( );
	sprintf_s( buf,20, "%04d/%02d/%02d %02d:%02d:%02d", sp.Year, sp.Month, sp.Day, sp.Hour, sp.Min, sp.Sec );
	return buf;
}

// 地方時間を返す
DateTime DateTime::ToLocalTime( int8_t p_nLocalHour = LOCAL_TIME_HOUR )
{
	DateTime RetVal = DateTime( *this );
	RetVal.Date += p_nLocalHour * SEC_HOUR;
	return RetVal;
}

// 日時構造体に変換する
DateTimeSt DateTime::ToStructur( )
{
	DateTimeSt stRetVal;
	uint32_t dwDate = ElapseDate() + 428; // 3月起点にするためにずらす
	uint32_t dwDayTemp;
	stRetVal.Year = dwDate * 400 / 146097; // 仮年(閏年を考慮)

	// 1/1からの経過日の計算
	while( 1 )//  11月～2月のみ２回ループ
	{
		dwDayTemp = ( dwDate - YEAR_DATE( stRetVal.Year ) ); // 1/1からの仮経過日
		if( dwDayTemp > 122 )
		{
			break;
		}
		stRetVal.Year--;
	}
	stRetVal.Month = ( dwDayTemp * 10 / 306 ) - 1; // 仮月

	// 日,月の計算
	while( 1 ) // 1,3,8月の31日のみ２回ループ
	{
		stRetVal.Day = dwDayTemp - MonthDate( stRetVal.Month ); // 仮日
		if( stRetVal.Day > 0 )
		{
			break;
		}
		stRetVal.Month--;
	}
	if( stRetVal.Month >= 13 )
	{
		stRetVal.Year++;
		stRetVal.Month -= 12;
	}
	// 時、分、秒の計算
	stRetVal.Hour = Hour( );// ( Date / SEC_HOUR ) % DAY_HOUR;
	stRetVal.Min = Min( );// ( Date / HOUR_MIN ) % MIN_SEC;
	stRetVal.Sec = Sec( );// Date % MIN_SEC;

	return stRetVal;
}

// UnixTimeに変換する
uint32_t DateTime::ToUnixTime( )
{
	return (uint32_t)( Date - UNIX_EPOCH );
}

// RTCレジスタ値に変換する
uint8_t * DateTime::ToRtcData( uint8_t *pRtcDate )
{
	DateTimeSt stData=ToStructur();

	pRtcDate[0] = BYTE2BCD( stData.Sec  );
	pRtcDate[1] = BYTE2BCD( stData.Min  );
	pRtcDate[2] = BYTE2BCD( stData.Hour  );
	pRtcDate[3] = WeekDay( );
	pRtcDate[4] = BYTE2BCD( stData.Day  );
	pRtcDate[5] = BYTE2BCD( stData.Month  );
	pRtcDate[6] = BYTE2BCD( stData.Year %100  );
	
	return pRtcDate;
}

// 0000年1月1日からの経過日を返す
// MAX : 
uint32_t DateTime::ElapseDate( )
{
	return (uint32_t)( Date / DAY_MAX );
}

// 曜日(0:日曜日～6:土曜日)を返す
uint8_t DateTime::WeekDay( )
{

	return ElapseDate() % 7;
}

// 西暦を返す
uint16_t DateTime::Year( )
{
	DateTimeSt stDate = ToStructur( );
	return stDate.Year;
}

// 月を返す
uint8_t DateTime::Month( )
{
	DateTimeSt stDate = ToStructur( );
	return stDate.Month;
}

// 日を返す
uint8_t DateTime::Day( )
{
	DateTimeSt stDate = ToStructur( );
	return stDate.Day;
}

// 時を返す
uint8_t DateTime::Hour( )
{
	return ( Date / SEC_HOUR ) % DAY_HOUR;
}

// 分を返す
uint8_t DateTime::Min( )
{
	return ( Date / HOUR_MIN ) % MIN_SEC;
}

// 秒を返す
uint8_t DateTime::Sec( )
{
	return Date % MIN_SEC;
}
