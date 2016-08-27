class GraphicChipInterface {
  public:
    char buff[32];
    void clear();
    void print(char*);
    void println(char*);
    void println();
    void draw_row(int, int, int, int);
    void set_cursor(int, int);

  private:
    void wait();
};
