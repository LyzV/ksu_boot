#include "packing.h"

/*
**************************************************
	—татические члены класса CFloatPacking
**************************************************
*/
bool order_defined=false;
bool mantissa_first=true;
static CFloatPacking FloatPacking;//экземпл€р дл€ установки флага "mantissa_first"

CFloatPacking::CFloatPacking()
{
	if(!order_defined)
   {
      if(MANTISSA_FIRST!=Order()) mantissa_first=false;
      else                        mantissa_first=true;
      order_defined=true;
   }
}

int CFloatPacking::Order()
{
   float one=-1.5;
   uint32_t container=FloatNormalForm(one);

   if(0<=Sign(container))
   {//далее только инверсна€ форма может быть корректной
      container=FloatInverseForm(one);
      if(0<=Sign(container)) return IT_IS_NOT_IEEE754;
      container=FloatInverseForm(-one);
      if(0>Sign(container)) return IT_IS_NOT_IEEE754;
      return MANTISSA_FIRST;
   }
   else
   {//далее могут быть обе формы корректны
      //провер€ю нормальную форму
      container=FloatNormalForm(-one);
      if(0>Sign(container))
      {//далее может быть корректна только инверсна€ форма
         container=FloatInverseForm(one);
         if(0<=Sign(container)) return IT_IS_NOT_IEEE754;
         container=FloatInverseForm(-one);
         if(0>Sign(container)) return IT_IS_NOT_IEEE754;
         return MANTISSA_FIRST;
      }
      return SIGN_FIRST;
   }
}

int CFloatPacking::Mantissa(uint32_t normal_form_val)
{
   int mantissa=0x007fffff&normal_form_val;
   if(Sign(normal_form_val))
      return mantissa;
   return -mantissa;
}

int CFloatPacking::Mantissa(float val)
{
	return Mantissa(FloatNormalForm(val));
}

int CFloatPacking::Exponent(uint32_t normal_form_val)
{
   int exponent=0x7f800000&normal_form_val;
   return (exponent>>23)-127;
}

int CFloatPacking::Sign(uint32_t normal_form_val)
{
   if(0x80000000&normal_form_val) return -1;
   return 1;
}

uint32_t CFloatPacking::FloatNormalForm(float val)
{
   uint32_t container;
   container =(uint32_t)(((uint8_t *)&val)[0])<<24;
   container+=(uint32_t)(((uint8_t *)&val)[1])<<16;
   container+=(uint32_t)(((uint8_t *)&val)[2])<<8;
   container+=(uint32_t)(((uint8_t *)&val)[3]);
   return container;
}

uint32_t CFloatPacking::FloatInverseForm(float val)
{
   uint32_t container;
   container =(uint32_t)(((uint8_t *)&val)[3])<<24;
   container+=(uint32_t)(((uint8_t *)&val)[2])<<16;
   container+=(uint32_t)(((uint8_t *)&val)[1])<<8;
   container+=(uint32_t)(((uint8_t *)&val)[0]);
   return container;
}

bool CFloatPacking::Check(float val)
{
   switch(Representation(val))
   {
   case float_ZERO:
   case float_DENORMAL:
   case float_NORMAL:
      return true;
   default: return false;
   }
}

int CFloatPacking::Representation(float val)
{
   uint32_t container;
   int mantissa;
   uint8_t E;//не экспонента а: exponent=E-127 => E=exponent+127

   if(mantissa_first)
   //if(MANTISSA_FIRST==Order())
      container=FloatInverseForm(val);
   else
      container=FloatNormalForm(val);

   mantissa= Mantissa(container);
   E=(uint8_t)(Exponent(container)+127);

   if(     !E &&!mantissa) return float_ZERO    ;
   if(     !E && mantissa) return float_DENORMAL;
   if((255==E)&&!mantissa) return float_INFINITY;
   if((255==E)&& mantissa) return float_NAN     ;
   
   return float_NORMAL;
}

int CFloatPacking::Pack(float val, void *buf)
{
    uint8_t *ptr=(uint8_t *)&val;
	if(mantissa_first)
	{
        ((uint8_t *)buf)[0]=*ptr++;
        ((uint8_t *)buf)[1]=*ptr++;
        ((uint8_t *)buf)[2]=*ptr++;
        ((uint8_t *)buf)[3]=*ptr;
	}
	else
	{
        ((uint8_t *)buf)[3]=*ptr++;
        ((uint8_t *)buf)[4]=*ptr++;
        ((uint8_t *)buf)[5]=*ptr++;
        ((uint8_t *)buf)[0]=*ptr;
	}
	
    return sizeof(float);
}

int CFloatPacking::Unpack(float &val, const void *buf)
{
    uint8_t *ptr=(uint8_t *)&val;
	if(mantissa_first)
	{
        *ptr++=((uint8_t *)buf)[0];
        *ptr++=((uint8_t *)buf)[1];
        *ptr++=((uint8_t *)buf)[2];
        *ptr  =((uint8_t *)buf)[3];
	}
	else
	{
        *ptr++=((uint8_t *)buf)[3];
        *ptr++=((uint8_t *)buf)[2];
        *ptr++=((uint8_t *)buf)[1];
        *ptr  =((uint8_t *)buf)[0];
	}
	
    return sizeof(float);
}

bool CFloatPacking::IsFloatEquel(float one, float two, float plus_minus)
{
	if(one==two)
	{
		return true;
	}
	else if(one>two)
	{
		if(one<two+plus_minus)
			return true;
	}
	else
	{
		if(one+plus_minus>two)
			return true;
	}
	return false;
}
