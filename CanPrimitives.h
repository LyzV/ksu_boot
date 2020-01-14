#ifndef CAN_PRIMITIVES
#define CAN_PRIMITIVES

#include "vtypes.h"
#include "packing.h"

/*
*********************************************************************
   ���������� ������ �������
*********************************************************************
*/
#define CAN_PHISICAL_KSU_ADDR		   1 //���
#define CAN_PHISICAL_KIZ_ADDR		   5 //��� ������
#define CAN_PHISICAL_DRIVE_ADDR        5
#define CAN_PHISICAL_KPT_ADDR		   3 //���
#define CAN_PHISICAL_KTM_ADDR		   2 //���

/*
***************************************************************************
   ���������� �� CAN-�
***************************************************************************
*/
//*** ������ ��� ���������� ���������� ���������
#define CAN_RESET_A_CMD										0xF000	//������� ������ �����, �� ����������
#define CAN_READ_NEW_SERIAL									0xDDD1
#define CAN_READ_NEW_SERIAL_BPT								0xDDD3
#define CAN_KIZ_PUSK_FRAME_READ_CMD		      				0x3507   //������� 5 ����� ��������� �������
#define CAN_KIZ_SYNC   										0x3101   // ������������� �����������
#define CAN_KPT_PERIODICALLY_READ_CMD    	   				0x3030	//������� �������������� ������ �������� SKPTPeriodicallyReadMsg	�� ���
#define CAN_KTM_PERIODICALLY_READ_CMD    	   				0x3020	//������� �������������� ������ �������� SKTMPeriodicallyReadMsg	�� ���

/*
***********************************************************************************************
    ���� ������
************************************************************************************************
*/
#define PRIM_NO_ERR			0 //��� � �����

#define PRIM_NO_ANSV_ERR	128 //������ ���
#define PRIM_PACK_SIZE_ERR	129 //�� ��������� �������� ��������� � �������
#define PRIM_CMD_ERR		130 //������ ��������� � �������� ��������
#define PRIM_NULL_PTR       131 //������� ���������
#define PRIM_SOCKET_ERR     132 //�������� ����� ������ �� CAN
#define PRIM_SEND_ERR       133 //������ ��� �������� �����
#define PRIM_RECV_ERR       134 //������ ��� ����� �����



#endif//CAN_PRIMITIVES
