#include "Tokenizer.h"
#include "Interpret.h"
#include <string.h>
#include <stdlib.h>
#if not defined (__AVR_ATmega328P__)
#include <iostream>
#endif // not

Tokenizer::Tokenizer(char buffer)
{
    tokens = (Token*) malloc((buffer + 2) * sizeof(Token));
    stringparam[0] = 0;
}

void Tokenizer::load(char* input)
{
    this->input = input;
}

bool isInteger(char* input)
{
    int16_t j = 0, k = strlen(input);
    if (k > 1 && input[0] == '-') j++;
    while(j < k)
    {
        if((input[j] > '9' || input[j] < '0'))
            return false;
        j++;
    }
    return true;
}

bool isSingleChar(char* buffer)
{
    return buffer[1] == 0 || buffer[1] == ' ' || buffer[1] == '\n';
}

bool isOperator(char* buffer)
{
    return buffer[0] == '+' || buffer[0] == '-' || buffer[0] == '*' || buffer[0] == '/';
}

bool isRelop(char* buffer)
{
    return buffer[0] == '=' || buffer[0] == '>' || buffer[0] == '<' || buffer[0] == '!';
}

bool isVar(char* buffer)
{
    return buffer[0] >= 'A' && buffer[0] <= ('A' + NUM_VARS);
}

bool isString(char* buffer)
{
    //std::cout << "[" << buffer << "]" <<  std::endl;
    return buffer[0] == '"' && strchr(buffer, '"');
}

char* blankOrNewline(char* ptr)
{
    char* ptrBlank = strchr(ptr,' ');
    char* ptrBreak = strchr(ptr,'\n');
    if (ptrBlank == NULL && ptrBreak == NULL)
    {
        return NULL;
    }
    else if (ptrBlank == NULL)
    {
        return ptrBreak;
    }
    else if (ptrBreak == NULL)
    {
        return ptrBlank;
    }
    else if (ptrBlank > ptrBreak)
    {
        return ptrBreak;
    }
    else
    {
        return ptrBlank;
    }
}

char Tokenizer::tokenize()
{
    return tokenize(0, false);
}

char Tokenizer::tokenize(char line, bool haltOnLineNumber)
{
    char * ptr;
    ptr = this->input;
    char i = 0;
    char skip = 0;

    while (skip < line)
    {
        ptr++;
        if (ptr[0] == '\n')
        {
            skip++;
            ptr++;
        }
    }

    stringparam[0] = 0;
    char buffer[STRNG_LEN];

    while (ptr!=NULL)
    {
        int16_t len = 0;
        if (ptr[0] == '"')
        {
            for (int i = 1; i < 20; i++) {
                if (ptr[i] == '"') {
                    len = i + 1;
                    break;
                }
            }
        }
        else {
            char* charPos = blankOrNewline(ptr);
            if (charPos == NULL)
            {
                len = 1;
            }
            else
            {
                len = charPos - ptr;
            }
        }
        memcpy(buffer, ptr, len);
        buffer[len] = 0;
        if (strlen(buffer) == 0)
        {
            break;
        }
        if (isInteger(buffer))
        {
            if (ptr == this->input || ptr[-1]== '\n')
            {
                if (atoi(buffer) > 0)
                {
                    if (haltOnLineNumber && (i != 0))
                    {
                        break;
                    }
                    this->tokens[i].type = LINENUMBER;
                    this->tokens[i++].value = atoi(buffer);
                }
            }
            else
            {
                this->tokens[i].type = NUMBER;
                this->tokens[i++].value = atoi(buffer);
            }
        }
        else if (strlen(buffer) == 1)
        {
            if (isOperator(buffer))
            {
                this->tokens[i].type = OPERATOR;
                this->tokens[i++].value = buffer[0];
            }
            else if (isRelop(buffer))
            {
                this->tokens[i].type = RELOP;
                this->tokens[i++].value = buffer[0];
            }
            else if (isVar(buffer))
            {
                this->tokens[i].type = VARIABLE;
                this->tokens[i++].value = buffer[0] - 'A';
            }
            else
            {
                // TODO: ERROR
            }
        }
        else if (isString(buffer))
        {
            memcpy(stringparam, buffer + 1, strlen(buffer) - 2);
            stringparam[strlen(buffer) - 2] = 0;
            this->tokens[i].type = STRING;
            this->tokens[i++].value = 0;
        }
        else if (buffer[0] == 'P')
        {
            this->tokens[i].type = KEYWORD;
            if (buffer[1] == 'R')
              this->tokens[i++].value = PRINT;
            if (buffer[1] == 'L')
              this->tokens[i++].value = PLINE;
        }
        else if (buffer[0] == 'L')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = LET;
        }
        else if (buffer[0] == 'G')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = GOTO;
        }
        else if (buffer[0] == 'I')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = IF;
        }
        else if (buffer[0] == 'E')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = END;
        }
        else if (buffer[0] == 'A')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = ASK;
        }
        else if (buffer[0] == 'R')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = RND;
        }
        else if (buffer[0] == 'F')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = FOR;
        }
        else if (buffer[0] == 'N')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = NEXT;
        }
        else if (buffer[0] == 'T')
        {
            this->tokens[i].type = KEYWORD;
            this->tokens[i++].value = TO;
        }
        ptr = blankOrNewline(ptr + len);
        if (ptr != NULL)
        {
            ptr++;
        }
    }
    this->tokens[i].type = PROGEND;
    this->tokens[i++].value = 0;
    return i;
}

char Tokenizer::findLineNumber(int16_t number)
{
    char line = 0;
    char * ptr;
    int16_t i = 0;
    ptr = this->input;
    char buffer[10];
    while (ptr!=NULL)
    {
        int16_t len = 0;
        char* charPos = blankOrNewline(ptr);
        if (charPos == NULL)
        {
            len = 1;
        }
        else
        {
            len = charPos - ptr;
        }
        memcpy(buffer, ptr, len);
        buffer[len] = 0;
        if (strlen(buffer) == 0)
        {
            break;
        }
        if (isInteger(buffer))
        {
            if (ptr == this->input || ptr[-1]== '\n')
            {
                if (atoi(buffer) > 0)
                {
                    if (atoi(buffer) == number)
                    {
                        return line;
                    }
                    line++;
                }
            }
        }
        ptr = blankOrNewline(ptr);
        if (ptr != NULL)
        {
            ptr++;
        }
    }
    return -1;
}

Tokenizer::~Tokenizer()
{
    free(tokens);
}
