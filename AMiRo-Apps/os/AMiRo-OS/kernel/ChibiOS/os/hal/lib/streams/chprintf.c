/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
   Concepts and parts of this file have been contributed by Fabio Utzig,
   chvprintf() added by Brent Roman.
 */

/**
 * @file    chprintf.c
 * @brief   Mini printf-like functionality.
 *
 * @addtogroup HAL_CHPRINTF
 * @details Mini printf-like functionality.
 * @{
 */

#include "hal.h"
#include "chprintf.h"
#include "memstreams.h"
#if CHPRINTF_USE_FLOAT
#include "math.h"
#endif

#if CHPRINTF_USE_FLOAT || CHPRINTF_SUPPORT_LONGLONG
#define MAX_FILLER 22
#else
#define MAX_FILLER 11
#endif
#define FLOAT_PRECISION 9

#if CHPRINTF_SUPPORT_LONGLONG
static char *longlong_to_string_with_divisor(char *p,
                                             long long num,
                                             unsigned radix,
                                             long divisor) {
  long long l, ll;
#else
static char *long_to_string_with_divisor(char *p,
                                         long num,
                                         unsigned radix,
                                         long divisor) {
  long l, ll;
#endif
  int i;
  char *q;

  l = num;
  if (divisor == 0) {
    ll = num;
  } else {
    ll = divisor;
  }

#if CHPRINTF_USE_FLOAT || CHPRINTF_SUPPORT_LONGLONG
  q = p + (MAX_FILLER/2);
#else
  q = p + MAX_FILLER;
#endif
  do {
    i = (int)(l % radix);
    i += '0';
    if (i > '9') {
      i += 'A' - '0' - 10;
    }
    *--q = i;
    l /= radix;
  } while ((ll /= radix) != 0);

#if CHPRINTF_USE_FLOAT || CHPRINTF_SUPPORT_LONGLONG
  i = (int)(p + (MAX_FILLER/2) - q);
#else
  i = (int)(p + MAX_FILLER - q);
#endif
  do
    *p++ = *q++;
  while (--i);

  return p;
}

#if CHPRINTF_SUPPORT_LONGLONG
static char *ch_lltoa(char *p, long long num, unsigned radix) {
  return longlong_to_string_with_divisor(p, num, radix, 0);
}
#else
static char *ch_ltoa(char *p, long num, unsigned radix) {
  return long_to_string_with_divisor(p, num, radix, 0);
}
#endif

#if CHPRINTF_USE_FLOAT
static const long pow10[FLOAT_PRECISION] = {
    10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

static char *ftoa(char *p, double num, unsigned long precision) {
  long l;

  if ((precision == 0) || (precision > FLOAT_PRECISION)) {
    precision = FLOAT_PRECISION;
  }
  precision = pow10[precision - 1];

  l = (long)num;
#if CHPRINTF_SUPPORT_LONGLONG
  p = longlong_to_string_with_divisor(p, l, 10, 0);
#else
  p = long_to_string_with_divisor(p, l, 10, 0);
#endif
  *p++ = '.';
  l = (long)((num - l) * precision);

#if CHPRINTF_SUPPORT_LONGLONG
  return longlong_to_string_with_divisor(p, l, 10, precision / 10);
#else
  return long_to_string_with_divisor(p, l, 10, precision / 10);
#endif
}
#endif

/**
 * @brief   System formatted output function.
 * @details This function implements a minimal @p vprintf()-like functionality
 *          with output on a @p BaseSequentialStream.
 *          The general parameters format is: %[-][width|*][.precision|*][l|L][l|L]p.
 *          The following parameter types (p) are supported:
 *          - <b>x</b> hexadecimal integer.
 *          - <b>X</b> hexadecimal long.
 *          - <b>o</b> octal integer.
 *          - <b>O</b> octal long.
 *          - <b>d</b> decimal signed integer.
 *          - <b>D</b> decimal signed long.
 *          - <b>u</b> decimal unsigned integer.
 *          - <b>U</b> decimal unsigned long.
 *          - <b>c</b> character.
 *          - <b>s</b> string.
 *          .
 *
 * @param[in] chp       pointer to a @p BaseSequentialStream implementing object
 * @param[in] fmt       formatting string
 * @param[in] ap        list of parameters
 * @return              The number of bytes that would have been
 *                      written to @p chp if no stream error occurs
 *
 * @api
 */
int chvprintf(BaseSequentialStream *chp, const char *fmt, va_list ap) {
  char *p, *s, c, filler;
  int i, precision, width;
  int n = 0;
  bool is_long, left_align, do_sign;
#if CHPRINTF_SUPPORT_LONGLONG
  bool is_longlong;
  long long l;
#else
  long l;
#endif
#if CHPRINTF_USE_FLOAT
  float f;
#endif
  char tmpbuf[MAX_FILLER + 1];

  while (true) {
    c = *fmt++;
    if (c == 0) {
      return n;
    }
    
    if (c != '%') {
      streamPut(chp, (uint8_t)c);
      n++;
      continue;
    }
    
    p = tmpbuf;
    s = tmpbuf;

    /* Alignment mode.*/
    left_align = false;
    if (*fmt == '-') {
      fmt++;
      left_align = true;
    }

    /* Sign mode.*/
    do_sign = false;
    if (*fmt == '+') {
      fmt++;
      do_sign = true;
    }

    /* Filler mode.*/
    filler = ' ';
    if (*fmt == '0') {
      fmt++;
      filler = '0';
    }
    
    /* Width modifier.*/
    if ( *fmt == '*') {
      width = va_arg(ap, int);
      ++fmt;
      c = *fmt++;
    }
    else {
      width = 0;
      while (true) {
        c = *fmt++;
        if (c == 0) {
          return n;
        }
        if (c >= '0' && c <= '9') {
          c -= '0';
          width = width * 10 + c;
        }
        else {
          break;
        }
      }
    }
    
    /* Precision modifier.*/
    precision = 0;
    if (c == '.') {
      c = *fmt++;
      if (c == 0) {
        return n;
      }
      if (c == '*') {
        precision = va_arg(ap, int);
        c = *fmt++;
      }
      else {
        while (c >= '0' && c <= '9') {
          c -= '0';
          precision = precision * 10 + c;
          c = *fmt++;
          if (c == 0) {
            return n;
          }
        }
      }
    }
    
    /* Long modifier.*/
    if (c == 'l' || c == 'L') {
      is_long = true;
      c = *fmt++;
      if (c == 0) {
        return n;
      }
    }
    else {
      is_long = (c >= 'A') && (c <= 'Z');
    }

#if CHPRINTF_SUPPORT_LONGLONG
    /* long long modifier */
    if (is_long && (c == 'l' || c == 'L')) {
      is_longlong = true;
      c = *fmt++;
      if (c == 0) {
        return n;
      }
    } else {
      is_longlong = false;
    }
#endif

    /* Command decoding.*/
    switch (c) {
    case 'c':
      filler = ' ';
      *p++ = va_arg(ap, int);
      break;
    case 's':
      filler = ' ';
      if ((s = va_arg(ap, char *)) == 0) {
        s = "(null)";
      }
      if (precision == 0) {
        precision = 32767;
      }
      for (p = s; *p && (--precision >= 0); p++)
        ;
      break;
    case 'D':
    case 'd':
    case 'I':
    case 'i':
#if CHPRINTF_SUPPORT_LONGLONG
      if (is_longlong) {
        l = va_arg(ap, long long);
      } else
#endif
      if (is_long) {
        l = va_arg(ap, long);
      }
      else {
        l = va_arg(ap, int);
      }
      if (l < 0) {
        *p++ = '-';
        l = -l;
      }
      else
        if (do_sign) {
          *p++ = '+';
        }
#if CHPRINTF_SUPPORT_LONGLONG
      p = ch_lltoa(p, l, 10);
#else
      p = ch_ltoa(p, l, 10);
#endif
      break;
#if CHPRINTF_USE_FLOAT
    case 'f':
      f = (float) va_arg(ap, double);
      if (isfinite(f)) {
        if (f < 0) {
          *p++ = '-';
          f = -f;
        }
        else {
          if (do_sign) {
            *p++ = '+';
          }
        }
        p = ftoa(p, f, precision);
      }
      else {
        const char* fpcode = isinf(f) ? "(inf)" : isnan(f) ? "(nan)" : "(err)";
        while (*fpcode != '\0') {
          *p++ = *fpcode++;
        }
      }
      break;
#endif
    case 'X':
    case 'x':
    case 'P':
    case 'p':
      c = 16;
      goto unsigned_common;
    case 'U':
    case 'u':
      c = 10;
      goto unsigned_common;
    case 'O':
    case 'o':
      c = 8;
unsigned_common:
#if CHPRINTF_SUPPORT_LONGLONG
      if (is_longlong) {
        l = va_arg(ap, unsigned long long);
      } else
#endif
      if (is_long) {
        l = va_arg(ap, unsigned long);
      }
      else {
        l = va_arg(ap, unsigned int);
      }
#if CHPRINTF_SUPPORT_LONGLONG
      p = ch_lltoa(p, l, c);
#else
      p = ch_ltoa(p, l, c);
#endif
      break;
    default:
      *p++ = c;
      break;
    }
    i = (int)(p - s);
    if ((width -= i) < 0) {
      width = 0;
    }
    if (left_align == false) {
      width = -width;
    }
    if (width < 0) {
      if ((*s == '-' || *s == '+') && filler == '0') {
        streamPut(chp, (uint8_t)*s++);
        n++;
        i--;
      }
      do {
        streamPut(chp, (uint8_t)filler);
        n++;
      } while (++width != 0);
    }
    while (--i >= 0) {
      streamPut(chp, (uint8_t)*s++);
      n++;
    }

    while (width) {
      streamPut(chp, (uint8_t)filler);
      n++;
      width--;
    }
  }
}

/**
 * @brief   System formatted output function.
 * @details This function implements a minimal @p printf() like functionality
 *          with output on a @p BaseSequentialStream.
 *          The general parameters format is: %[-][width|*][.precision|*][l|L]p.
 *          The following parameter types (p) are supported:
 *          - <b>x</b> hexadecimal integer.
 *          - <b>X</b> hexadecimal long.
 *          - <b>o</b> octal integer.
 *          - <b>O</b> octal long.
 *          - <b>d</b> decimal signed integer.
 *          - <b>D</b> decimal signed long.
 *          - <b>u</b> decimal unsigned integer.
 *          - <b>U</b> decimal unsigned long.
 *          - <b>c</b> character.
 *          - <b>s</b> string.
 *          .
 *
 * @param[in] chp       pointer to a @p BaseSequentialStream implementing object
 * @param[in] fmt       formatting string
 * @return              The number of bytes that would have been
 *                      written to @p chp if no stream error occurs
 *
 * @api
 */
int chprintf(BaseSequentialStream *chp, const char *fmt, ...) {
  va_list ap;
  int formatted_bytes;

  va_start(ap, fmt);
  formatted_bytes = chvprintf(chp, fmt, ap);
  va_end(ap);

  return formatted_bytes;
}

/**
 * @brief   System formatted output function.
 * @details This function implements a minimal @p snprintf()-like functionality.
 *          The general parameters format is: %[-][width|*][.precision|*][l|L]p.
 *          The following parameter types (p) are supported:
 *          - <b>x</b> hexadecimal integer.
 *          - <b>X</b> hexadecimal long.
 *          - <b>o</b> octal integer.
 *          - <b>O</b> octal long.
 *          - <b>d</b> decimal signed integer.
 *          - <b>D</b> decimal signed long.
 *          - <b>u</b> decimal unsigned integer.
 *          - <b>U</b> decimal unsigned long.
 *          - <b>c</b> character.
 *          - <b>s</b> string.
 *          .
 * @post    @p str is NUL-terminated, unless @p size is 0.
 *
 * @param[in] str       pointer to a buffer
 * @param[in] size      maximum size of the buffer
 * @param[in] fmt       formatting string
 * @return              The number of characters (excluding the
 *                      terminating NUL byte) that would have been
 *                      stored in @p str if there was room.
 *
 * @api
 */
int chsnprintf(char *str, size_t size, const char *fmt, ...) {
  va_list ap;
  int retval;

  /* Performing the print operation.*/
  va_start(ap, fmt);
  retval = chvsnprintf(str, size, fmt, ap);
  va_end(ap);

  /* Return number of bytes that would have been written.*/
  return retval;
}

/**
 * @brief   System formatted output function.
 * @details This function implements a minimal @p vsnprintf()-like functionality.
 *          The general parameters format is: %[-][width|*][.precision|*][l|L]p.
 *          The following parameter types (p) are supported:
 *          - <b>x</b> hexadecimal integer.
 *          - <b>X</b> hexadecimal long.
 *          - <b>o</b> octal integer.
 *          - <b>O</b> octal long.
 *          - <b>d</b> decimal signed integer.
 *          - <b>D</b> decimal signed long.
 *          - <b>u</b> decimal unsigned integer.
 *          - <b>U</b> decimal unsigned long.
 *          - <b>c</b> character.
 *          - <b>s</b> string.
 *          .
 * @post    @p str is NUL-terminated, unless @p size is 0.
 *
 * @param[in] str       pointer to a buffer
 * @param[in] size      maximum size of the buffer
 * @param[in] fmt       formatting string
 * @param[in] ap        list of parameters
 * @return              The number of characters (excluding the
 *                      terminating NUL byte) that would have been
 *                      stored in @p str if there was room.
 *
 * @api
 */
int chvsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
  MemoryStream ms;
  BaseSequentialStream *chp;
  size_t size_wo_nul;
  int retval;

  if (size > 0)
    size_wo_nul = size - 1;
  else
    size_wo_nul = 0;

  /* Memory stream object to be used as a string writer, reserving one
     byte for the final zero.*/
  msObjectInit(&ms, (uint8_t *)str, size_wo_nul, 0);

  /* Performing the print operation using the common code.*/
  chp = (BaseSequentialStream *)(void *)&ms;
  retval = chvprintf(chp, fmt, ap);

  /* Terminate with a zero, unless size==0.*/
  if (ms.eos < size) {
    str[ms.eos] = 0;
  }

  /* Return number of bytes that would have been written.*/
  return retval;
}

/** @} */
