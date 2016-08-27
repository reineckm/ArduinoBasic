/* Basic for Arduino
 * 
 * @author reineckm
 * @license Apache-2.0
 */

#define __AVR_ATmega328P__ 1

#include <EEPROM.h>
#include <PS2Keyboard.h>
#include <Wire.h>

#include <MLBuffer.h>
#include <fontALL.h>
#include <Tokenizer.h>
#include <Interpret.h>
#include <GraphicChipInterface.h>

#define SBW 29
#define SBH 9
#define FONT font4x6
#define FONTW 4
#define FONTH 6
#define BUFSIZE 999

char lineBuf[SBW];
byte cX, cY, startLine;

// + 1 da Buffer plus 0
MLBuffer mlb = MLBuffer(BUFSIZE, SBW + 1);
Tokenizer* tok = new Tokenizer(10);
Interpret interpret = Interpret(tok);
PS2Keyboard Keyboard;

GraphicChipInterface GC;

void requestEvent() {
  digitalWrite(13, HIGH);
  Wire.write(GC.buff);
  GC.buff[0] = '9';
  digitalWrite(13, LOW);
}

void setup()  {
  Wire.begin(8);
  Wire.onRequest(requestEvent);
  Keyboard.begin(3, 2, PS2Keymap_German);
  GC.buff[0] = '9';
  clearAll();
  GC.print("Awesome Basic Ready!");
}

void loop() {
  if (Keyboard.available()) {
    remCursor();
    char c = Keyboard.read();
    switch (c) {
      case PS2_LEFTARROW: curLeft(); break;
      case PS2_DOWNARROW: curDown(); break;
      case PS2_UPARROW: curUp(); break;
      case PS2_RIGHTARROW: curRight(); break;
      case PS2_ENTER: onEnter(); break;
      case PS2_BACKSPACE: onBackspace(); break;
      default:
        if (c == ' ')
          c = '_';
        if (cX < SBW) {
          insertCharInLineBuf(cX, c);
        }
        curRight();
        printSB(cY, cY);
    }
    drawCursor();
  }
}

void onEnter() {
  if (!commands()) {
    writeLineToBuffer(startLine + cY);
    if (cY < (SBH - 1)) {
      cY++;
    } else {
      startLine++;
    }
    mlb.newline(startLine + cY);
    mlb.set(startLine + cY, "");
    updateLineBuf();
    cX = 0;
    printSB(0, SBH);
  }
}

void onBackspace() {
  if (cX == 0) {
    if (isLineBufEmpty()) {
      mlb.del(startLine + cY);
    }
    if (cY > 0) {
      cY--;
    } else if (startLine > 0) {
      startLine--;
    } else {
      return; // Oben Links angekommen
    }
    cX = updateLineBuf();
    printSB(0, SBH);
  } else {
    curLeft();
    deleteCharInLineBuf(cX);
    printSB(cY, cY);
  }
}

void curUp() {
  bool fullRefresh = false;
  writeLineToBuffer(startLine + cY);
  if (cY > 0) {
    cY--;
  } else if (startLine > 0) {
    startLine--;
    fullRefresh = true;
  }
  int last = updateLineBuf();
  if (last < cX)
    cX = last;
  if (fullRefresh) {
    printSB(0, SBH);
  } else {
    printSB(cY, cY + 1);
  }
}

void curDown() {
  bool fullRefresh = false;
  writeLineToBuffer(startLine + cY);
  if (cY < (SBH - 1)) {
    cY++;
  } else {
    startLine++;
    fullRefresh = true;
  }
  int last = updateLineBuf();
  if (last < cX)
    cX = last;
  if (fullRefresh) {
    printSB(0, SBH);
  } else {
    printSB(cY - 1, cY);
  }
}

void curLeft() {
  if (cX > 0) {
    cX--;
  } else {
    curUp();
    cX = lineBufLen();
  }
}

void curRight() {
  bool fullRefresh = false;
  if (cX < (lineBufLen()) + 1) {
    cX++;
  } else {
    writeLineToBuffer(startLine + cY);
    if (cY < (SBH - 1)) {
      cY++;
    } else {
      startLine++;
      fullRefresh = true;
    }
    updateLineBuf();
    cX = 0;
    if (fullRefresh) {
      printSB(0, SBH);
    } else {
      printSB(cY - 1, cY);
    }
  }
}

int getInt() {
  int inX = 0;
  char inBuf[5] = {0, 0, 0, 0, 0};
  GC.clear();
  GC.set_cursor(0, 0);
  GC.print(interpret.output());
  while (true) {
    if (Keyboard.available()) {
      char c = Keyboard.read();
      if (c == '\r') {
        GC.println();
        return atoi(inBuf);
      } else if (c == 127 && cX > 0) {
        inX--;
        inBuf[inX] = ' ';
      } else if (cX < 5) {
        inBuf[inX] = c;
        inX++;
      }
      GC.set_cursor(0, 1);
      GC.print(inBuf);
    }
  }
}

boolean commands() {
  if (strncmp(lineBuf, "!S", 2) == 0) {
    saveAll();
    clearLineBuf();
    cX = 0;
  } else if (strncmp(lineBuf, "!L", 2) == 0) {
    clearAll();
    readAll();
    cX = 0;
    cY = 0;
    startLine = 0;
    updateLineBuf();
    printSB(0, SBH);
  } else if  (strncmp(lineBuf, "!C", 2) == 0) {
    clearAll();
    printSB(0, SBH);
  } else if  (strncmp(lineBuf, "!R", 2) == 0) {
    runBas();
  } else {
    return false;
  }
  return true;
}

void runBas() {
  interpret.load(mlb.text);
  GC.clear();
  GC.set_cursor(0, 0);
  while (interpret.run()) {
    if (interpret.requestUserInput) {
      interpret.input(getInt());
    }
    else if (interpret.outputAvaiable) {
      GC.print(interpret.output());
    }
  }
  while (!Keyboard.available());
    Keyboard.read();
  GC.println(interpret.output());
  printSB(0, SBH);
}

bool isLineBufEmpty() {
  for (int i = 0; i < SBW - 1; i++) {
    if (lineBuf[i] != ' ')
      return false;
  }
  return true;
}

void deleteCharInLineBuf(int cX) {
  for (int i = cX; i < SBW - 1; i++) {
    lineBuf[i] = lineBuf[i + 1];
  }
  lineBuf[SBW - 2] = ' ';
}

void insertCharInLineBuf(int cX, char c) {
  for (int i = SBW - 1; i > cX; i--) {
    lineBuf[i] = lineBuf[i - 1];
  }
  lineBuf[cX] = c;
}

int lineBufLen() {
  int l = 0;
  for (int i = 0; i < SBW; i++) {
    if (lineBuf[i] != ' ')
      l = i + 1;
  }
  return l;
}

void writeLineToBuffer(int line) {
  for (int i = 0; i < SBW; i++) {
    if (lineBuf[i] == '_')
      lineBuf[i] = ' ';
  }
  int l = lineBufLen();
  char* newText = new char[l + 1];
  memcpy(newText, lineBuf, l);
  newText[l] = '\0';
  mlb.set(line, newText);
  free(newText);
}

int updateLineBuf() {
  char* aktLine = mlb.getFrom(startLine + cY);
  // Kopiere ohne NULL
  clearLineBuf();
  for (int i = 0; i < SBW; i++) {
    if (aktLine[i] == '\n') {
      return i;
    }
    lineBuf[i] = aktLine[i];
  }
}

void drawCursor() {
  GC.draw_row(cY * FONTH + FONTW + 1, cX * FONTW, cX * FONTW + FONTW, 1);
}

void remCursor() {
  GC.draw_row(cY * FONTH + FONTW + 1, 0, 120, 0);
}

void printSB(int from, int to) {
  char b[SBW];
  for (int y = from; y <= to; y++) {
    for (int x = 0; x < SBW; x++) {
      b[x] = ' ';
    }
    if (y == cY) {
      for (int x = 0; x < SBW; x++) {
        if (lineBuf[x] == '\0' || lineBuf[x] == '\n' || lineBuf[x] == '_') {
          b[x] = ' ';
        } else {
          b[x] = lineBuf[x];
        }
      }
    } else {
      char* akt = mlb.getFrom(startLine + y);
      for (int x = 0; x < SBW; x++) {
        if (akt[x] == '\0' || akt[x] == '\n') {
          b[x] = ' ';
          break;
        } else {
          b[x] = akt[x];
        }
      }
    }
    GC.set_cursor(0, y * FONTH);
    GC.print(b);
  }
  GC.set_cursor(0, SBH * FONTH);
  char statusLine[SBW];
  sprintf(statusLine, "%-20s|R%3dC%3d", interpret.output(), freeRam(), mlb.bytesLeft());
  for (int i = 0; i < SBW; i++) {
    if (statusLine[i] == '\n')
      statusLine[i] = ' ';
  }
  GC.print(statusLine);
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void clearLineBuf() {
  for (int x = 0; x < SBW; x++) {
    lineBuf[x] = ' ';
  }
}

char toUpper(char in) {
  if (in >=  'a' && in <= 'z') {
    return in - 32;
  }
  return in;
}

void saveAll() {
  int addr = 2, i = 0;
  while (mlb.text[i] != '\0') {
    EEPROM.write(addr++, mlb.text[i++]);
  }
  EEPROM.write(addr++, 0);
  EEPROM.put(0, i);
}

void readAll() {
  int numBytes;
  int addr = 2;
  EEPROM.get(0, numBytes);
  for (int i = 0; i < numBytes; i++) {
    mlb.text[i] = EEPROM.read(addr++);
  }
}

void clearAll() {
  for (int i = 0; i < BUFSIZE; i++) {
    mlb.text[i] = 0;
  }
  cX = 0;
  cY = 0;
  startLine = 0;
  clearLineBuf();
}
