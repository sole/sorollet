#include "MathUtils.h"

int MathUtils::randSeed = 1;

float MathUtils::normalize(float value, float minimum, float maximum)
{
	return (value - minimum) / (maximum - minimum);
}

float MathUtils::interpolate(float normValue, float minimum, float maximum)
{
	return minimum + (maximum - minimum) * normValue;
}

float MathUtils::map(float value, float in_min, float in_max, float out_min, float out_max)
{
    if(in_min == in_max)
    {
        return out_min;
    }

    return out_min + (out_max - out_min) * (value - in_min) / (in_max - in_min);
}

float MathUtils::maxf(float value1, float value2)
{
    return( value1 > value2 ? value1 : value2);
}

float MathUtils::minf(float value1, float value2)
{
    return( value1 < value2 ? value1 : value2);
}

float MathUtils::randf()
{
    randSeed *= 16807;
    return ((float)randSeed) / (float)0x80000000;
}

float MathUtils::clipf(float value, float minV, float maxV)
{	
	if(value < minV)
	{
		value = minV;
	}
	else if(value > maxV)
	{
		value = maxV;
	}
	
	return value;
}

long MathUtils::floor(double value)
{
	// trick from http://code4k.blogspot.com/2009/10/random-float-number-using-x86-asm-code.html#more
	#ifdef WINDOWS
		__asm cvttsd2si eax,value
	#else
		return floor(value);
	#endif
}
