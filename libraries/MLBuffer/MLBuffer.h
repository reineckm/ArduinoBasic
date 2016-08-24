class MLBuffer {
  public:
    char* text;
    bool set(int line, char* text);
    bool add(char* text);
    bool del(int line);
    bool newline(int line);
    char* getFrom(int line);
    int bytesLeft();
    int getLines();
    MLBuffer(int bufferSize, char maxLineSize);
  private:
    int maxSize;
    char maxLineSize;
    char lineLength(int start);
    int firstFreeByte();
    int getPosAt(int line);
    void pad(int start, int bytes);
    void eat(int start, int bytes);
};
