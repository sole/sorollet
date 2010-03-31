/* 
 * File:   math_utils.h
 * Author: sole
 *
 * Created on 08 February 2010, 21:18
 */

#ifndef _MATH_UTILS_H
#define	_MATH_UTILS_H

class MathUtils
{
public:
    static float normalize(float value, float minimum, float maximum);
    static float interpolate(float normValue, float minimum, float maximum);
    static float map(float value, float in_min, float in_max, float out_min, float out_max);
    static float maxf(float value1, float value2);
    static float minf(float value1, float value2);
    static float randf();
	static float clipf(float value, float minV, float maxV);
    static int randSeed;
    static long floor(double value);
};

#ifndef M_PI
#define M_PI 3.1415927f
#endif

#ifndef M_2_PI
#define M_2_PI 0.6366197f
#endif

#endif	/* _MATH_UTILS_H */

