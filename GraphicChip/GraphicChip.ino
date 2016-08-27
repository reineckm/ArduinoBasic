/* Graphic Chip for Basic for Arduino
 * 
 * @author reineckm
 * @license Apache-2.0
 */
 
#include <Wire.h>
#include <TVout.h>
#include <fontALL.h>

#define __AVR_ATmega328P__ 1
#define SBW 29
#define SBH 9
#define FONT font4x6
#define FONTW 4
#define FONTH 6

TVout TV;

void setup()  {
  TV.begin(_NTSC, SBW * FONTW + FONTW, SBH * FONTH + FONTH);
  TV.select_font(FONT);
  Wire.begin();
}

int a, b, c, d;
char buff[35];
char subbuff[4];
byte byteBuff;
void loop() {
  getCmd();
}

void getCmd() {
  Wire.requestFrom(8, 32);
  int i = 0;
  while (Wire.available()) {
    char c = Wire.read();
    if (c == '\a') {
      buff[i++] = '\0';
    } else {
      buff[i++] = c;
    }
  }
  doCmd();
}

void doCmd() {
  switch (buff[0]) {
    case '0':
      TV.clear_screen();
      break;
    case '1':
      TV.print(&buff[1]);
      break;
    case '2':
      TV.println(&buff[1]);
      break;
    case '3':
      memcpy( subbuff, &buff[1], 3 );
      subbuff[3] = '\0';
      a = atoi(subbuff);
      memcpy( subbuff, &buff[4], 3 );
      b = atoi(subbuff);
      memcpy( subbuff, &buff[7], 3 );
      c = atoi(subbuff);
      memcpy( subbuff, &buff[10], 1 );
      subbuff[1] = '\0';
      d = atoi(subbuff);
      TV.draw_row(a, b, c, d);
      break;
    case '4':
      memcpy( subbuff, &buff[1], 2 );
      subbuff[2] = '\0';
      a = atoi(subbuff);
      memcpy( subbuff, &buff[3], 2 );
      b = atoi(subbuff);
      TV.set_cursor(a, b);
      break;
    case '5':
      byteBuff = TV.get_cursor_x();
      break;
    case '6':
      byteBuff = TV.get_cursor_y();
      break;
    case '9':
      TV.delay_frame(3);
      break;
  }
}
