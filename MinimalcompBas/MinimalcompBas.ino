#define __AVR_ATmega328P__ 1

#include <EEPROM.h>
#include <TVout.h>
#include <PS2Keyboard.h>
#include <MLBuffer.h>
#include <fontALL.h>
#include <Tokenizer.h>
#include <Interpret.h>

#define SBW 29
#define SBH 9
#define BUFSIZE 400
#define FONT font4x6
#define FONTW 4
#define FONTH 6

char lineBuf[SBW];
byte cX, cY, startLine;

// + 1 da Buffer plus 0
MLBuffer mlb = MLBuffer(BUFSIZE, SBW + 1);
Tokenizer* tok = new Tokenizer(10);
Interpret interpret = Interpret(tok);
TVout TV;
PS2Keyboard keyboard;

void demo() {
  clearAll();
  readAll();
  updateLineBuf();
  runBas();
}

void setup()  {
  //keyboard.begin(2, 5, PS2Keymap_German);
  Serial.begin(9600);
  TV.begin(NTSC, SBW * FONTW + FONTW, SBH * FONTH + FONTH);
  TV.select_font(FONT);
  clearAll();
  updateLineBuf();
  //demo();
  printSB(0,SBH);
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
  //TV.clear_screen();
  //TV.set_cursor(0, 0);
  TV.print(interpret.output());
  int x = TV.get_cursor_x();
  int y = TV.get_cursor_y();
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\r') {
        TV.println();
        return atoi(inBuf);
      } else if (c == 127 && cX > 0) {
        inX--;
        inBuf[inX] = ' ';
      } else if (cX < 5) {
        inBuf[inX] = c;
        inX++;
      }
      TV.set_cursor(x, y);
      TV.print(inBuf);
    }
  }
}

int escStage = 0;
void loop() {
  if (Serial.available()) {
    remCursor();
    char c = Serial.read();
    if (c == 27) {
      escStage = 1;
    } else if (c == '[' && escStage == 1) {
      escStage = 2;
    } else if (escStage == 2) {
      switch (c) {
        case 'D': curLeft(); break;
        case 'B': curDown(); break;
        case 'A': curUp(); break;
        case 'C': curRight(); break;
      }
      escStage = 0;
    } else if (c == 127) {
      onBackspace();
    } else if (c == '\r') {
      onEnter();
    } else {
      if (c == ' ')
        c = '_';
      if (cX < SBW) {
        insertCharInLineBuf(cX, toUpper(c));
      }
      curRight();
      printSB(cY, cY);  
    
    }
    drawCursor();
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
  TV.clear_screen();
  TV.set_cursor(0, 0);
  while (interpret.run()) {
    if (interpret.requestUserInput) {
      interpret.input(getInt());
    }
    else if (interpret.outputAvaiable) {
      TV.print(interpret.output());
    }
  }
  while (!Serial.available());
  Serial.read();
  TV.println(interpret.output());
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
/*
void updateSB() {
  TV.set_cursor(0, cY * FONTH);
  for (int x = 0; x < SBW; x++) {
    TV.print(lineBuf[x]);
  }
}
*/
void drawCursor() {
    TV.draw_row(cY * FONTH + FONTW + 1, cX * FONTW, cX * FONTW + FONTW, 1);
}

void remCursor() {
    TV.draw_row(cY * FONTH + FONTW + 1, 0, 120, 0);
}

void printSB(int from, int to) {
  TV.set_cursor(0, from * FONTH);
  for (int y = from; y <= to; y++) {
    char* akt;
    if (y == cY) {
      akt = lineBuf;
    } else {
      akt = mlb.getFrom(startLine + y);
    }
    bool fillBlank = false;
    for (int x = 0; x < SBW; x++) {
      if (akt[x] == '\0' || akt[x] == '\n')
        fillBlank = true;
      if (fillBlank) {
        TV.print(' ');
      } else {
        if (akt[x] == '_') 
          akt[x] = ' ';
        TV.print(akt[x]);
      }
    }
    if (y < (SBH - 2)) TV.println("");
  }
  TV.set_cursor(0, SBH * FONTH);
  char statusLine[SBW];
  sprintf(statusLine, "%-20s|R%3dC%3d", interpret.output(), freeRam(), mlb.bytesLeft());
  for (int i = 0; i < SBW; i++) {
    if (statusLine[i] == '\n')
      statusLine[i] = ' ';
  }
  TV.print(statusLine);
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
