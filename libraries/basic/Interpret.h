#ifndef INTERPRET_H
#define INTERPRET_H
#include "Token.h"
#include "Tokenizer.h"
#include "stdint.h"
#define NUM_VARS 10

class Interpret
{
    public:
        Interpret(Tokenizer *tokenizer);
        void load(char* programm);
        bool run();
        char* output();
        void input(int16_t in);
        bool requestUserInput;
        bool outputAvaiable;
        virtual ~Interpret();

    protected:
        Token* tokenBuf;
        int16_t line;
        char userInputVariable;
        char msg[STRNG_LEN + 1];

    private:
        Tokenizer* tokenizer;
        bool running;
        int16_t vars[NUM_VARS];
        int16_t forLineVars[NUM_VARS];
        bool forRunning[NUM_VARS];
        char fillTokenBuf();
        char next(char from);
        void debugTokenBuf();
        void doTerm();
        void doExpr();
        void doCommands();
        void doRelation();
        void varToNum(char pos);
        bool cmdPrint(char bufferStart);
        bool cmdGoto(char bufferStart);
        bool cmdAsk(char bufferStart);
        bool cmdRnd(char bufferStart);
        bool cmdLet(char bufferStart);
        bool cmdFor(char bufferStart);
        bool cmdNext(char bufferStart);
};

#endif // INTERPRET_H
