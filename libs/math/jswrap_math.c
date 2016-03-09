/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * Contains built-in functions for Maths
 * ----------------------------------------------------------------------------
 */
#include "jswrap_math.h"
#include "jsvariterator.h"

static bool isNegativeZero(double x) {
  double NEGATIVE_ZERO = -0.0;
  return *((long long*)&x) == *((long long*)&NEGATIVE_ZERO);
}

double jswrap_math_sin(double x) {
#ifdef SAVE_ON_FLASH
  /* To save on flash, do our own sin function that's slower/nastier
   * but is smaller! If we pull in gcc's it adds:
   * __kernel_rem_pio2    2054 bytes
   * __ieee754_rem_pio2   1060 bytes
   * __kernel_cos         584 bytes
   * __kernel_sin         364 bytes
   *
   * So about 3k, just for sin.
   * */
  // exploit symmetry - we're only accurate when x is small
  int xi = (int)(x/PI);
  x -= xi*PI;
  if (x>PI/2) x=PI-x;
  // Taylor series expansion of 'sin'
  double r = x; // running total
  double x2 = x*x; // precalculate x^2
  double xpow = x; // running power
  unsigned int factorial = 1; // running factorial
  unsigned int i;
  for (i=1;i<10;i++) {
    xpow = xpow*x2;
    factorial *= (i*2)*((i*2)+1);
    double term = xpow / factorial;
    if (i&1) r-=term; else r+=term;
  }
  // symmetry
  if (xi&1) r=-r;
  return r;
#else
  return sin(x);
#endif
}

/*JSON{
  "type" : "object",
  "name" : "Math",
  "memberOf" : "global"
}
This is a standard JavaScript class that contains useful Maths routines
*/

// -------------------------------------------------------------------- Math
/*JSON{
  "type" : "variable",
  "name" : "E",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "2.718281828459045",
  "return" : ["float","The value of E - 2.718281828459045"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "PI",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "PI",
  "return" : ["float","The value of PI - 3.141592653589793"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "LN2",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "0.6931471805599453",
  "return" : ["float","The natural logarithm of 2 - 0.6931471805599453"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "LN10",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "2.302585092994046",
  "return" : ["float","The natural logarithm of 10 - 2.302585092994046"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "LOG2E",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "1.4426950408889634",
  "return" : ["float","The base 2 logarithm of e - 1.4426950408889634"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "LOG10E",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "0.4342944819032518",
  "return" : ["float","The base 10 logarithm of e - 0.4342944819032518"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "SQRT2",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "1.4142135623730951",
  "return" : ["float","The square root of 2 - 1.4142135623730951"]
}

*/
/*JSON{
  "type" : "variable",
  "name" : "SQRT1_2",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "0.7071067811865476",
  "return" : ["float","The square root of 1/2 - 0.7071067811865476"]
}

*/


/*JSON{
  "type" : "function",
  "name" : "abs",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_abs",
  "params" : [
    ["x","float","A floating point value"]
  ],
  "return" : ["float","The absolute value of x (eg, ```Math.abs(2)==2```, but also ```Math.abs(-2)==2```)"]
}

*/
JsVarFloat jswrap_math_abs(JsVarFloat x) {
  return (x<0)?-x:x;
}
/*JSON{
  "type" : "function",
  "name" : "acos",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "jswrap_math_atan(jswrap_math_sqrt(1-x*x) / x)",
  "params" : [
    ["x","float","The value to get the arc cosine of"]
  ],
  "return" : ["float","The arc cosine of x, between 0 and PI"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "asin",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "jswrap_math_atan(x / jswrap_math_sqrt(1-x*x))",
  "params" : [
    ["x","float","The value to get the arc sine of"]
  ],
  "return" : ["float","The arc sine of x, between -PI/2 and PI/2"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "atan",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_atan",
  "params" : [
    ["x","float","The value to get the arc tangent  of"]
  ],
  "return" : ["float","The arc tangent of x, between -PI/2 and PI/2"]
}

*/
double jswrap_math_atan(double x) {
#ifdef SAVE_ON_FLASH
  /* To save on flash, do our own atan function that's slower/nastier
   * but is smaller! */
  // exploit symmetry - we're only accurate when x is small
  double ox = x;
  bool negate = false;
  bool offset = false;
  if (x<0) {
    x = -x;
    negate = true;
  }
  if (x>1) {
    x = 1/x;
    offset = true;
  }

  // Taylor series expansion of 'atan'
  double r = x; // running total
  double x2 = x*x; // precalculate x^2
  double xpow = x; // running power
  unsigned int i;
  for (i=1;i<20;i++) {
    xpow = xpow*x2;
    double term = xpow / ((i*2)+1);
    if (i&1) r-=term; else r+=term;
  }
  // symmetry
  if (offset) r=(PI/2)-r;
  if (negate) r=-r;
  return r;
#else
  return atan(x);
#endif
}

/*JSON{
  "type" : "function",
  "name" : "atan2",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "atan2",
  "params" : [
    ["y","float","The Y-part of the angle to get the arc tangent of"],
    ["x","float","The X-part of the angle to get the arc tangent of"]
  ],
  "return" : ["float","The arctangent of Y/X, between -PI and PI"],
  "if" : "!defined(SAVE_ON_FLASH)"
}

*/

/* we use sin here, not cos, to try and save a bit of code space */
/*JSON{
  "type" : "function",
  "name" : "cos",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "jswrap_math_sin(theta + (PI/2))",
  "params" : [
    ["theta","float","The angle to get the cosine of"]
  ],
  "return" : ["float","The cosine of theta"]
}

*/

double jswrap_math_mod(double x, double y) {
  double a, b;
  const double c = x;

  if (!isfinite(x) || isnan(y))
    return NAN;

  if (0 > c) {
    x = -x;
  }
  if (0 > y) {
    y = -y;
  }
  if (y != 0 && DBL_MAX >= y && DBL_MAX >= x) {
    while (x >= y) {
      a = x / 2;
      b = y;
      while (a >= b) {
        b *= 2;
      }
      x -= b;
    }
  } else {
    x = 0;
  }
  return 0 > c ? -x : x;
}

double jswrap_math_pow(double x, double y) {
  double p;
  /* quick hack for raising to a small integer power.
   * exp/log aren't accurate and are relatively slow, so
   * it's probably better to bash through small integer
   * powers in a stupid way. */
  int yi = (int)y;
  if (yi>=0 && yi<10 && yi==y) {
    if (yi==0) return 1.0;
    p = x;
    while (yi>1) {
      p *= x;
      yi--;
    }
    return p;
  }

  /* do proper floating point pow. Not as accurate as a
   * proper pow implementation but this saves a *lot*
   * of flash */
  if (x < 0 && jswrap_math_mod(y, 1) == 0) {
    if (jswrap_math_mod(y, 2) == 0) {
      p = exp(log(-x) * y);
    } else {
      p = -exp(log(-x) * y);
    }
  } else {
    if (x != 0 || 0 >= y) {
      p = exp(log( x) * y);
    } else {
      p = 0;
    }
  }
  return p;
}


/*JSON{
  "type" : "function",
  "name" : "pow",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_pow",
  "params" : [
    ["x","float","The value to raise to the power"],
    ["y","float","The power x should be raised to"]
  ],
  "return" : ["float","x raised to the power y (x^y)"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "random",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "(JsVarFloat)rand() / (JsVarFloat)RAND_MAX",
  "return" : ["float","A random number between 0 and 1"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "round",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_round",
  "params" : [
    ["x","float","The value to round"]
  ],
  "return" : ["JsVar","x, rounded to the nearest integer"]
}

*/
JsVar *jswrap_math_round(double x) {
  if (!isfinite(x) || isNegativeZero(x)) return jsvNewFromFloat(x);
  x += (x<0) ? -0.4999999999 : 0.4999999999;
  JsVarInt i = (JsVarInt)x;
  if (i==0 && (x<0))
    return jsvNewFromFloat(-0.0); // pass -0 through
  return jsvNewFromInteger(i);
}

/*JSON{
  "type" : "function",
  "name" : "sin",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_sin",
  "params" : [
    ["theta","float","The angle to get the sine of"]
  ],
  "return" : ["float","The sine of theta"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "tan",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "jswrap_math_sin(theta) / jswrap_math_sin(theta+(PI/2))",
  "params" : [
    ["theta","float","The angle to get the tangent of"]
  ],
  "return" : ["float","The tangent of theta"]
}

*/ // note - we're trying to avoid having to use cos here

/* we could use the real sqrt - but re-use pow to save on code space */
/*JSON{
  "type" : "function",
  "name" : "sqrt",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_sqrt",
  "params" : [
    ["x","float","The value to take the square root of"]
  ],
  "return" : ["float","The square root of x"]
}

*/

double jswrap_math_sqrt(double x) {
  return (x>=0) ? exp(log(x) * 0.5) : NAN;
}

/*JSON{
  "type" : "function",
  "name" : "ceil",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "ceil",
  "params" : [
    ["x","float","The value to round up"]
  ],
  "return" : ["float","x, rounded upwards to the nearest integer"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "floor",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "floor",
  "params" : [
    ["x","float","The value to round down"]
  ],
  "return" : ["float","x, rounded downwards to the nearest integer"]
}

*/

/*JSON{
  "type" : "function",
  "name" : "exp",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "exp",
  "params" : [
    ["x","float","The value raise E to the power of"]
  ],
  "return" : ["float","E^x"]
}

*/
/*JSON{
  "type" : "function",
  "name" : "log",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "log",
  "params" : [
    ["x","float","The value to take the logarithm (base E) root of"]
  ],
  "return" : ["float","The log (base E) of x"]
}

*/

/*JSON{
  "type" : "function",
  "name" : "clip",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "jswrap_math_clip",
  "params" : [
    ["x","float","A floating point value to clip"],
    ["min","float","The smallest the value should be"],
    ["max","float","The largest the value should be"]
  ],
  "return" : ["float","The value of x, clipped so as not to be below min or above max."],
  "if" : "!defined(SAVE_ON_FLASH)"
}
DEPRECATED - Please use `E.clip()` instead. Clip a number to be between min and max (inclusive)
*/
JsVarFloat jswrap_math_clip(JsVarFloat x, JsVarFloat min, JsVarFloat max) {
  if (x<min) x=min;
  if (x>max) x=max;
  return x;
}

/*JSON{
  "type" : "function",
  "name" : "wrap",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate" : "wrapAround",
  "params" : [
    ["x","float","A floating point value to wrap"],
    ["max","float","The largest the value should be"]
  ],
  "return" : ["float","The value of x, wrapped so as not to be below min or above max."],
  "if" : "!defined(SAVE_ON_FLASH)"
}
Wrap a number around if it is less than 0 or greater than or equal to max. For instance you might do: ```Math.wrap(angleInDegrees, 360)```
*/

/*JSON{
  "type" : "function",
  "name" : "min",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "jswrap_math_minmax(args, false)",
  "params" : [
    ["args","JsVarArray","A floating point value to clip"]
  ],
  "return" : ["float","The minimum of the supplied values"]
}
Find the minimum of a series of numbers
*/
/*JSON{
  "type" : "function",
  "name" : "max",
  "memberOf" : "Math",
  "thisParam" : false,
  "generate_full" : "jswrap_math_minmax(args, true)",
  "params" : [
    ["args","JsVarArray","A floating point value to clip"]
  ],
  "return" : ["float","The maximum of the supplied values"]
}
Find the maximum of a series of numbers
*/
JsVarFloat jswrap_math_minmax(JsVar *args, bool isMax) {
  JsVarFloat v = isMax ? -INFINITY : INFINITY;

  JsvObjectIterator it;
  jsvObjectIteratorNew(&it, args);
  while (jsvObjectIteratorHasValue(&it)) {
    JsVarFloat arg = jsvGetFloatAndUnLock(jsvObjectIteratorGetValue(&it));
    if ((isMax && arg > v) || (!isMax && arg < v) || isnan(arg))
      v = arg;
    jsvObjectIteratorNext(&it);
  }
  jsvObjectIteratorFree(&it);

  return v;
}

