#pragma once
#define globalVariable static
#define internal static


inline int clamp( int val, int min, int max ) {
    if ( val < min ) return min;
    if ( val > max ) return max;
    return val;
}