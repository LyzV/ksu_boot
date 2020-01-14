#ifndef LV_PACKING
#define LV_PACKING

#include "vtypes.h"
/*
*********************************************************************
   Интерфейс ураковки/разпаковки
*********************************************************************
*/
struct IPacking
{
	virtual int Pack    (void *buf) const =0;
	virtual int Unpack  (void *buf)=0;
	virtual int PackSize(         ) const =0;
};

/*
**************************************************
    Класс для определения порядка упаковки float
**************************************************
*/
// *** Порядок упаковки float
#define MANTISSA_FIRST     0
#define SIGN_FIRST         1
#define IT_IS_NOT_IEEE754  -1

// *** Определения для проверки float
#define float_ZERO       0x1   //число -0
#define float_DENORMAL   0x2   //число ненормализованное
#define float_NORMAL     0x4   //нормальное число
#define float_INFINITY   0x8   //бесконечность
#define float_NAN        0x10  //NaN (Not A Number) - ноль деленный на ноль

class CFloatPacking
{
   static uint32_t FloatNormalForm (float val);
   static uint32_t FloatInverseForm(float val);

public:
	CFloatPacking();

   static int  Order();
   static bool Check(float val);
   static int  Representation(float val);//представление float
    static int  Pack(float val	, void *buf);
    static int  Unpack(float &val	, const void *buf);
   static int  Sign(uint32_t normal_form_val);//знак
   static int  Mantissa(uint32_t normal_form_val);//знаком вначале памяти
   static int  Mantissa(float val);
   static int  Exponent(uint32_t normal_form_val);//знаком вначале памяти
   static bool IsFloatEquel(float one, float two, float plus_minus);
};

/*
**************************************************
	Класс для упаковки
**************************************************
*/
class CPacking
{
public:

	//*** Упаковать
    static int Pack(int8_t val, void *buf)
	{
        ((int8_t *)buf)[0]=val;
        return sizeof(int8_t);
	}
    static int Pack(uint8_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=val;
        return sizeof(uint8_t);
	}
    static int Pack(bool val, void *buf, int)
	{
       uint32_t _val=(uint32_t)val;
	   return Pack(_val, buf);
	}
	
    static int Pack(int16_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=(uint8_t)val;
        ((uint8_t *)buf)[1]=(uint8_t)(val>>8);
        return sizeof(int16_t);
	}
    static int Pack(uint16_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=(uint8_t)val;
        ((uint8_t *)buf)[1]=(uint8_t)(val>>8);
        return sizeof(uint16_t);
	}
	
    static int Pack(int32_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=(uint8_t)val;
        ((uint8_t *)buf)[1]=(uint16_t)(val>>8);
        ((uint8_t *)buf)[2]=(uint32_t)(val>>16);
        ((uint8_t *)buf)[3]=(uint32_t)(val>>24);
        return sizeof(int32_t);
	}
    static int Pack(uint32_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=(uint8_t)val;
        ((uint8_t *)buf)[1]=(uint16_t)(val>>8);
        ((uint8_t *)buf)[2]=(uint32_t)(val>>16);
        ((uint8_t *)buf)[3]=(uint32_t)(val>>24);
        return sizeof(uint32_t);
	}
    static int Pack(int64_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=(uint8_t)val;
        ((uint8_t *)buf)[1]=(uint16_t)(val>>8);
        ((uint8_t *)buf)[2]=(uint32_t)(val>>16);
        ((uint8_t *)buf)[3]=(uint32_t)(val>>24);
		
        ((uint8_t *)buf)[4]=(uint64_t)(val>>32);
        ((uint8_t *)buf)[5]=(uint64_t)(val>>40);
        ((uint8_t *)buf)[6]=(uint64_t)(val>>48);
        ((uint8_t *)buf)[7]=(uint64_t)(val>>56);
		
        return sizeof(int32_t);
	}
    static int Pack(uint64_t val, void *buf)
	{
        ((uint8_t *)buf)[0]=(uint8_t)val;
        ((uint8_t *)buf)[1]=(uint16_t)(val>>8);
        ((uint8_t *)buf)[2]=(uint32_t)(val>>16);
        ((uint8_t *)buf)[3]=(uint32_t)(val>>24);

        ((uint8_t *)buf)[4]=(uint64_t)(val>>32);
        ((uint8_t *)buf)[5]=(uint64_t)(val>>40);
        ((uint8_t *)buf)[6]=(uint64_t)(val>>48);
        ((uint8_t *)buf)[7]=(uint64_t)(val>>56);

        return sizeof(int32_t);
	}
    static int Pack(float val, void *buf)
	{
		return CFloatPacking::Pack(val, buf);
	}
	
    static int Pack(const float *val, int val_sz, void *buf)
	{
		int ret=0;
		for(int i=0; i<val_sz; i++)
            ret+=Pack(*(val+i), (uint8_t *)buf+ret);
		return ret;	
	}
	
	//*** Разпаковать
    static int Unpack(int8_t &val, const void *buf)
	{
		int ret;
        uint8_t v;
		ret=Unpack(v, buf);
        val=(int8_t)v;
		return ret;
	}
    static int Unpack(uint8_t &val, const void *buf)
	{
        val=((uint8_t *)buf)[0];
        return sizeof(uint8_t);
	}
    static int Unpack(bool &val, const void *buf, int)
	{
	   int ret;
        uint32_t v;
		ret=Unpack(v, buf);
        val=(bool)v;
		return ret;
	}
    static int Unpack(int16_t &val, const void *buf)
	{
		int ret;
        uint16_t v;
		ret=Unpack(v, buf);
        val=(int16_t)v;
		return ret;
	}
    static int Unpack(uint16_t &val, const void *buf)
	{
        val =        ((uint8_t *)buf)[0];
        val+=(uint16_t)((uint8_t *)buf)[1]<<8;
        return sizeof(uint16_t);
	}
	
    static int Unpack(int32_t &val, const void *buf)
	{
		int ret;
        uint32_t v;
		ret=Unpack(v, buf);
        val=(int32_t)v;
		return ret;
	}
    static int Unpack(uint32_t &val, const void *buf)
	{
        val =        ((uint8_t *)buf)[0];
        val+=(uint16_t)((uint8_t *)buf)[1]<<8;
        val+=(uint32_t)((uint8_t *)buf)[2]<<16;
        val+=(uint32_t)((uint8_t *)buf)[3]<<24;
        return sizeof(uint32_t);
	}
    static int Unpack(int64_t &val, const void *buf)
	{
		int ret;
        uint64_t v;
		ret=Unpack(v, buf);
        val=(int64_t)v;
		return ret;
	}
    static int Unpack(uint64_t &val, const void *buf)
	{
        val =        ((uint8_t *)buf)[0];
        val+=(uint16_t)((uint8_t *)buf)[1]<<8;
        val+=(uint32_t)((uint8_t *)buf)[2]<<16;
        val+=(uint32_t)((uint8_t *)buf)[3]<<24;

        val+=(uint64_t)((uint8_t *)buf)[4]<<32;
        val+=(uint64_t)((uint8_t *)buf)[5]<<40;
        val+=(uint64_t)((uint8_t *)buf)[6]<<48;
        val+=(uint64_t)((uint8_t *)buf)[7]<<56;

        return sizeof(uint64_t);
	}
    static int Unpack(float &val, const void *buf)
	{
		return  CFloatPacking::Unpack(val, buf);
	}
    static int Unpack(float *val, int val_sz, const void *buf)
	{
		int ret=0;
        float v;
		for(int i=0; i<val_sz; i++)
		{
            ret+=Unpack(v, (uint8_t *)buf+ret);
			*(val+i)=v;
		}	
		return ret;	
	}
};


#endif
