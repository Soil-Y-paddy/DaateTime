#pragma once
#include<stdio.h>
#include<stdint.h>

#define 	LOCAL_TIME_HOUR		9	// GMT����

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
	uint64_t Date; // 0/1/1���N�_�Ƃ����o�ߕb

	DateTime( ){}
	DateTime( const DateTime & );
	DateTime( DateTimeSt ); // �����\���̂���̐���
	DateTime( uint32_t, uint32_t); //GPS���Ԃ��琶��
	DateTime( uint32_t ); // UnixTime����̐���
	DateTime( uint8_t*); // RTC���W�X�^����̐���


	char* ToString( char* buf );

	DateTime ToLocalTime( int8_t ); // �n�����Ԃɕϊ�����
	DateTimeSt ToStructur( ); // �����\���̂ɕϊ�����
	uint32_t ToUnixTime( ); // Unix���Ԃɕϊ�����
	uint8_t* ToRtcData( uint8_t* );//RTC���W�X�^�l�ɕϊ�����

	uint32_t ElapseDate( );// 0/1/1����̌o�ߓ�
	uint8_t	WeekDay( );//�j��
	uint16_t Year( ); // �N
	uint8_t Month( ); // ��
	uint8_t Day( );//��
	uint8_t Hour( );//��
	uint8_t Min( );//��
	uint8_t Sec( );//�b

};