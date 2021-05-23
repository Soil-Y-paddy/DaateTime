#pragma once
#include<stdio.h>
#include<stdint.h>

#define 	LOCAL_TIME_HOUR		9	// GMT時差

struct DateTimeSt
{
public:
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Min;
	uint8_t Sec;
	DateTimeSt( ) :Year( 0 ), Month( 0 ), Day( 0 ), Hour( 0 ), Min( 0 ), Sec( 0 ){}
};

class DateTime
{
private:
	static uint64_t CalcDate( DateTimeSt );
	void FromStructure( DateTimeSt );
public:
	uint64_t Date; // 0/1/1を起点とした経過秒

	DateTime( ){}
	DateTime( const DateTime & );
	DateTime( DateTimeSt ); // 日時構造体からの生成
	DateTime( uint32_t, uint32_t); //GPS時間から生成
	DateTime( uint32_t ); // UnixTimeからの生成
	DateTime( uint8_t*); // RTCレジスタからの生成


	char* ToString( char* buf );

	DateTime ToLocalTime( int8_t ); // 地方時間に変換する
	DateTimeSt ToStructur( ); // 日時構造体に変換する
	uint32_t ToUnixTime( ); // Unix時間に変換する
	uint8_t* ToRtcData( uint8_t* );//RTCレジスタ値に変換する

	uint32_t ElapseDate( );// 0/1/1からの経過日
	uint8_t	WeekDay( );//曜日
	uint16_t Year( ); // 年
	uint8_t Month( ); // 月
	uint8_t Day( );//日
	uint8_t Hour( );//時
	uint8_t Min( );//分
	uint8_t Sec( );//秒

};