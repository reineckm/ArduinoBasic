/*
TVPrint.cpp - Base class that provides print() and println()
Copyright (c) 2008 David A. Mellis.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Modified 23 November 2006 by David A. Mellis
*/

#include <math.h>
#include <avr/pgmspace.h>

#include "TVout.h"
#include <fontALL.h>

/***************************************************************
 
 Functions added in 1.0 LMI release
 
 ***************************************************************/

// Using set_font will set both the "font" variable that is set in select_font(),
// and font_int variable that is later used in print centered functions.
void TVout::set_font(uint8_t f)
{
    font_int_x = f;
    
    switch (f) {
        case 4:
            font = font4x6;
            font_int_y = 6;
            break;
        case 6:
            font = font6x8;
            font_int_y = 8;
            break;
        case 8:
            font = font8x8;
            font_int_y = 8;
            break;
            
        default:
            break;
    }
}
/*  get the x font width  */
uint8_t TVout::get_font_x()
{
    return font_int_x;
}
/*  get the y font height  */
uint8_t TVout::get_font_y()
{
    return font_int_y;
}
/*  get x value to center text on screen  */
uint8_t TVout::get_centered_x(char *temp)
{
    uint8_t x = (twidth - strlen(temp)*get_font_x())/2;
    // Never return a negative number
    if (x < 0) {
        x = 0;
    }
    
    return x;
}
/*  get y value to center text on screen  */
uint8_t TVout::get_centered_y(char *temp)
{
    uint8_t x = (theight + (get_font_y()/2))/2;
    return x;
}
/* get x resolution */
uint8_t TVout::get_max_x()
{
    return twidth;
}
/* get y resolution */
uint8_t TVout::get_max_y()
{
    return theight;
}
/* get last x cursor position */
uint8_t TVout::get_max_cursor_x()
{
    uint8_t x = (twidth-1) - get_font_x();
    return x;
}
/* get last y cursor position */
uint8_t TVout::get_max_cursor_y()
{
    uint8_t x = (theight-1) - get_font_y();
    return x;
}
/* Sets x of the current line such that text
   will be centered, the prints normally. */
void TVout::print_centered(char c[])
{
    // Set the cursor so the text is centered
    // on the current line.
    cursor_x = (int)get_centered_x(c);
    print(c);
}
void TVout::println_centered(char c[])
{
    // Set the cursor so the text is centered
    // on the current line.
    cursor_x = (int)get_centered_x(c);
    println(c);
}


/***************************************************************

    Functions from the original Beta1 release
 
 ***************************************************************/

void TVout::select_font(const unsigned char * f) {
	font = f;
}


/*
 * print an 8x8 char c at x,y
 * x must be a multiple of 8
 */
void TVout::print_char(uint8_t x, uint8_t y, unsigned char c) {

	c -= pgm_read_byte(font+2);
	bitmap(x,y,font,(c*pgm_read_byte(font+1))+3,pgm_read_byte(font),pgm_read_byte(font+1));
}

void TVout::inc_txtline() {
	if (cursor_y >= (display.vres - pgm_read_byte(font+1)))
		shift(pgm_read_byte(font+1),UP);
	else
		cursor_y += pgm_read_byte(font+1);
}

/* default implementation: may be overridden */
void TVout::write(const char *str)
{
  while (*str)
    write(*str++);
}

/* default implementation: may be overridden */
void TVout::write(const uint8_t *buffer, uint8_t size)
{
  while (size--)
    write(*buffer++);
}

void TVout::write(uint8_t c) {
	switch(c) {
		case '\0':			//null
			break;
		case '\n':			//line feed
			cursor_x = 0;
			inc_txtline();
			break;
		case 8:				//backspace
			cursor_x -= pgm_read_byte(font);
			print_char(cursor_x,cursor_y,' ');
			break;
		case 13:			//carriage return !?!?!?!VT!?!??!?!
			cursor_x = 0;
			break;
		case 14:			//form feed new page(clear screen)
			//clear_screen();
			break;
		default:
			if (cursor_x >= (display.hres*8 - pgm_read_byte(font))) {
				cursor_x = 0;
				inc_txtline();
				print_char(cursor_x,cursor_y,c);
			}
			else
				print_char(cursor_x,cursor_y,c);
			cursor_x += pgm_read_byte(font);
	}
}

void TVout::print(const char str[])
{
  write(str);
}

void TVout::print(char c, int base)
{
  print((long) c, base);
}

void TVout::print(unsigned char b, int base)
{
  print((unsigned long) b, base);
}

void TVout::print(int n, int base)
{
  print((long) n, base);
}

void TVout::print(unsigned int n, int base)
{
  print((unsigned long) n, base);
}

void TVout::print(long n, int base)
{
  if (base == 0) {
    write(n);
  } else if (base == 10) {
    if (n < 0) {
      print('-');
      n = -n;
    }
    printNumber(n, 10);
  } else {
    printNumber(n, base);
  }
}

void TVout::print(unsigned long n, int base)
{
  if (base == 0) write(n);
  else printNumber(n, base);
}

void TVout::print(double n, int digits)
{
  printFloat(n, digits);
}

void TVout::println(void)
{
  print('\r');
  print('\n');  
}

void TVout::println(const char c[])
{
  print(c);
  println();
}

void TVout::println(char c, int base)
{
  print(c, base);
  println();
}

void TVout::println(unsigned char b, int base)
{
  print(b, base);
  println();
}

void TVout::println(int n, int base)
{
  print(n, base);
  println();
}

void TVout::println(unsigned int n, int base)
{
  print(n, base);
  println();
}

void TVout::println(long n, int base)
{
  print(n, base);
  println();
}

void TVout::println(unsigned long n, int base)
{
  print(n, base);
  println();
}

void TVout::println(double n, int digits)
{
  print(n, digits);
  println();
}

void TVout::printPGM(const char str[]) {
	char c;
	while ((c = pgm_read_byte(str))) {
		str++;
		write(c);
	}
}

void TVout::printPGM(uint8_t x, uint8_t y, const char str[]) {
	char c;
	cursor_x = x;
	cursor_y = y;
	while ((c = pgm_read_byte(str))) {
		str++;
		write(c);
	}
}

void TVout::set_cursor(uint8_t x, uint8_t y) {
	cursor_x = x;
	cursor_y = y;
}

void TVout::print(uint8_t x, uint8_t y, const char str[]) {
	cursor_x = x;
	cursor_y = y;
	write(str);
	
}
void TVout::print(uint8_t x, uint8_t y, char c, int base) {
	cursor_x = x;
	cursor_y = y;
	print((long) c, base);
}
void TVout::print(uint8_t x, uint8_t y, unsigned char b, int base) {
	cursor_x = x;
	cursor_y = y;
	print((unsigned long) b, base);
}
void TVout::print(uint8_t x, uint8_t y, int n, int base) {
	cursor_x = x;
	cursor_y = y;
	print((long) n, base);
}
void TVout::print(uint8_t x, uint8_t y, unsigned int n, int base) {
	cursor_x = x;
	cursor_y = y;
	print((unsigned long) n, base);
}
void TVout::print(uint8_t x, uint8_t y, long n, int base) {
	cursor_x = x;
	cursor_y = y;
	print(n,base);
}
void TVout::print(uint8_t x, uint8_t y, unsigned long n, int base) {
	cursor_x = x;
	cursor_y = y;
	print(n,base);
}
void TVout::print(uint8_t x, uint8_t y, double n, int digits) {
	cursor_x = x;
	cursor_y = y;
	print(n,digits);
}

void TVout::println(uint8_t x, uint8_t y, const char c[])
{
	cursor_x = x;
	cursor_y = y;
	print(c);
	println();
}

void TVout::println(uint8_t x, uint8_t y, char c, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(c, base);
	println();
}

void TVout::println(uint8_t x, uint8_t y, unsigned char b, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(b, base);
	println();
}

void TVout::println(uint8_t x, uint8_t y, int n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void TVout::println(uint8_t x, uint8_t y, unsigned int n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void TVout::println(uint8_t x, uint8_t y, long n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void TVout::println(uint8_t x, uint8_t y, unsigned long n, int base)
{
	cursor_x = x;
	cursor_y = y;
	print(n, base);
	println();
}

void TVout::println(uint8_t x, uint8_t y, double n, int digits)
{
	cursor_x = x;
	cursor_y = y;
	print(n, digits);
	println();
}

void TVout::printNumber(unsigned long n, uint8_t base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  unsigned long i = 0;

  if (n == 0) {
    print('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    print((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void TVout::printFloat(double number, uint8_t digits) 
{ 
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint; 
  } 
}

uint8_t TVout::get_cursor_x(void)
{
    return cursor_x;
}

uint8_t TVout::get_cursor_y(void)
{
    return cursor_y;
}
