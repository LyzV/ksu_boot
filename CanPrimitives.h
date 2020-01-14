#ifndef CAN_PRIMITIVES
#define CAN_PRIMITIVES

#include "vtypes.h"
#include "packing.h"

/*
*********************************************************************
   ФИЗИЧЕСКИЕ АДРЕСА ЖЕЛЕЗОК
*********************************************************************
*/
#define CAN_PHISICAL_KSU_ADDR		   1 //ксу
#define CAN_PHISICAL_KIZ_ADDR		   5 //киз модуль
#define CAN_PHISICAL_DRIVE_ADDR        5
#define CAN_PHISICAL_KPT_ADDR		   3 //кпт
#define CAN_PHISICAL_KTM_ADDR		   2 //ктм

/*
***************************************************************************
   Сообщениея по CAN-у
***************************************************************************
*/
//*** группа для управления автоматами устройств
#define CAN_RESET_A_CMD										0xF000	//команда сброса блока, не используем
#define CAN_READ_NEW_SERIAL									0xDDD1
#define CAN_READ_NEW_SERIAL_BPT								0xDDD3
#define CAN_KIZ_PUSK_FRAME_READ_CMD		      				0x3507   //Считать 5 точек пускового графика
#define CAN_KIZ_SYNC   										0x3101   // Синхронизация осцилографа
#define CAN_KPT_PERIODICALLY_READ_CMD    	   				0x3030	//команда периодического чтения структры SKPTPeriodicallyReadMsg	из КПТ
#define CAN_KTM_PERIODICALLY_READ_CMD    	   				0x3020	//команда периодического чтения структры SKTMPeriodicallyReadMsg	из КТМ

/*
***********************************************************************************************
    Кода ошибок
************************************************************************************************
*/
#define PRIM_NO_ERR			0 //все в норме

#define PRIM_NO_ANSV_ERR	128 //ответа нет
#define PRIM_PACK_SIZE_ERR	129 //не совпадает принятый размерчик с должным
#define PRIM_CMD_ERR		130 //пришло сообщение с неверной командой
#define PRIM_NULL_PTR       131 //нулевой указатель
#define PRIM_SOCKET_ERR     132 //неверный сокет обмена по CAN
#define PRIM_SEND_ERR       133 //ошибка при отправке кадра
#define PRIM_RECV_ERR       134 //ошибка при приёме кадра



#endif//CAN_PRIMITIVES
