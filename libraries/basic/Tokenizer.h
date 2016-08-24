#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "Token.h"
#include "stdint.h"
#define STRNG_LEN 20

class Tokenizer
{
    public:
        Tokenizer(char buffer);
        char tokenize(char line, bool haltOnLineNumber);
        char tokenize();
        char findLineNumber(int16_t number);
        void load(char* input);
        char* input;
        char stringparam[STRNG_LEN];
        Token* tokens;
        virtual ~Tokenizer();

    protected:

    private:
};

#endif // TOKENIZER_H
