#include "MLBuffer.h"
#include <stdio.h>
#include <string.h>
#if not defined(ARDUINO)
  #include <iostream>
#endif // not

MLBuffer::MLBuffer(int bufferSize, char maxLineSize) {
  this->text = new char[bufferSize];
  this->maxSize = bufferSize;
  this->maxLineSize = maxLineSize;
  for(int i = 0; i < this->maxSize; i++)
    this->text[i] = 0;
}

int MLBuffer::firstFreeByte() {
  int last = 0;
  for(int i = 0; i < this->maxSize; i++) {
    if (this->text[i] != 0)
      last = i;
  }
  if (last >= this->maxSize) return -1;
  return last;
}

int MLBuffer::bytesLeft() {
  int first = this->firstFreeByte();
  if (first == -1) return 0;
  return (this->maxSize - first);
}

char MLBuffer::lineLength(int start) {
  for (int i = start; i < (this->maxLineSize + start); i++) {
    if (this->text[i] == '\n')
      return i - start;
  }
  return - 1;
}

void MLBuffer::pad(int start, int bytes) {
  char c1, c2;
  start++;
  for (int pads = 0; pads < bytes; pads++) {
    c1 = text[start];
    c2 = text[start + 1];
    this->text[start + 1] = text[start];
    for (int i = start; i < (this->maxSize - 1); i++) {
      this->text[i + 1] = c1;
      c1 = c2;
      c2 = text[i + 2];
    }
  }
}

void MLBuffer::eat(int start, int bytes) {
  for (int eats = 0; eats < bytes; eats++) {
    for (int i = start; i < (this->maxSize - 1); i++) {
      this->text[i] = this->text[i + 1];
    }
  }
}

bool MLBuffer::add(char* text) {
  char len = strlen(text);
  if (len > maxLineSize) return false;
  if (len > this->bytesLeft()) return false;
  int first = this->firstFreeByte();
  if (first != 0) first++;
  for (int i = 0; i < len; i++) {
    this->text[first + i] = text[i];
  }
  this->text[first + len] = '\n';
  return true;
}

int MLBuffer::getPosAt(int line) {
  int i = 0;
  int actLine = 0;
  while (i < this->maxSize) {
    if (actLine == line) return i;
    if (this->text[i] == '\n') actLine++;
    i++;
  }
  return -1;
}

int MLBuffer::getLines() {
  int i = 0;
  int actLine = 0;
  while (i < this->maxSize) {
    if (this->text[i] == '\n') actLine++;
    i++;
  }
  return actLine;
}

char* MLBuffer::getFrom(int line) {
    int i = this->getPosAt(line);
    if (i == -1)
        return this->text + this->maxSize - 1;
  return this->text + i;
}

bool MLBuffer::set(int line, char* text) {
  int start = this->getPosAt(line);
  if (start == -1) return false;
  char len = strlen(text);
  char space = this->lineLength(start);
  if (space < len) {
    this->pad(start, (len - space));
  } else if (space > len) {
    this->eat(start, (space - len));
  }
  for (int i = 0; i < len; i++) {
    this->text[start + i] = text[i];
  }
  this->text[start + len] = '\n';
  return true;
}

bool MLBuffer::newline(int line) {
  int start = this->getPosAt(line);
  if (start == -1) return false;
  char len = strlen(text);
  char space = this->lineLength(start);
  this->pad(start - 1, 1);
  this->text[start] = '\n';
  return true;
}

bool MLBuffer::del(int line) {
  int start = this->getPosAt(line);
  char space = this->lineLength(start);
  this->eat(start, space + 1);
}

#if not defined(ARDUINO)
void debugOut(MLBuffer mlb) {
    std::cout << "Text\n";
    std::cout << mlb.text;
    int i = 0;
    char c = mlb.text[i++];
    //while (c != 0) {
    //  std::cout << (int)c << ", " << c << '\n';
    //  c = mlb.text[i++];
    //}
}

int main() {
    MLBuffer mlb = MLBuffer(200, 19);
    mlb.add((char*)"Anfang");
    mlb.add((char*)"Mitte");
    mlb.add((char*)"Mitte2");
    mlb.add((char*)"Loesch Mich");
    mlb.add((char*)"Mi t t e3");
    mlb.add((char*)"Ende   ");
    mlb.del(3);
    mlb.set(0, (char*)"Begin");
    mlb.set(4, (char*)"End");
    mlb.set(2, (char*)"Mittendrin");
    mlb.set(5, (char*)"Dub");
    if (!mlb.set(7, (char*)"Dub")) {
      std::cout << "Test bestanden\n";
    }
    if (strcmp(mlb.text, "Begin\nMitte\nMittendrin\nMi t t e3\nEnd\nDub\n") == 0) {
      std::cout << "Test bestanden\n";
    }
    if (strcmp(mlb.getFrom(3), "Mi t t e3\nEnd\nDub\n") == 0) {
      std::cout << "Test bestanden\n";
    }
    if (strcmp(mlb.getFrom(9), "") == 0) {
      std::cout << "Test bestanden\n";
    }
    if (mlb.getLines() == 6) {
      std::cout << "Test bestanden\n";
    }
    debugOut(mlb);
    mlb.newline(3);
    debugOut(mlb);
    mlb.newline(=);
    debugOut(mlb);
    if (mlb.getLines() == 7) {
      std::cout << "Test bestanden\n";
    }

    std::cout << mlb.getFrom(9);

}
#endif // defined
