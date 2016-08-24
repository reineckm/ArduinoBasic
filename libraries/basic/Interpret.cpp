#include "Interpret.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#if not defined(__AVR_ATmega328P__)
#include <iostream>
#else
#include "Arduino.h"
#endif

Interpret::Interpret(Tokenizer *tokenizer)
{
    this->tokenizer = tokenizer;
    for (int i = 0; i < NUM_VARS; i++) {
        vars[i] = 0;
        forLineVars[i] = 0;
        forRunning[i] = false;
    }
}

void Interpret::load(char* programm)
{
    this->tokenizer->load(programm);
    this->line = 0;
    this->msg[0] = 0;
    this->running = true;
    this->requestUserInput = false;
    this->outputAvaiable = false;
}

bool Interpret::run()
{
    if ((fillTokenBuf() > 1) && running)
    {
        doTerm();
        doExpr();
        doRelation();
        doCommands();
        return true;
    }
    else
    {
        return false;
    }
}

char Interpret::fillTokenBuf()
{
    char i = this->tokenizer->tokenize(this->line, true);
    tokenBuf = this->tokenizer->tokens;
    this->line++;
    return i;
}

char Interpret::next(char from)
{
    char i = from + 1;
    while (this->tokenBuf[i].type < PROGEND)
    {
        if (this->tokenBuf[i].type == LINENUMBER)
        {
            return 0;
        }
        else if (this->tokenBuf[i].type == EMPTY)
        {
            i++;
        }
        else
        {
            return i;
        }
    }
    return 0;
}

void Interpret::doTerm()
{
    int16_t lhs = 0, op, rhs;
    while (true)
    {
        op = this->next(lhs);
        if (op == 0) break;
        rhs = this->next(op);
        if ((tokenBuf[lhs].type == NUMBER || tokenBuf[lhs].type == VARIABLE) &&
                (tokenBuf[op].type == OPERATOR) &&
                (tokenBuf[op].value == '*' || tokenBuf[op].value == '/') &&
                (tokenBuf[rhs].type == NUMBER || tokenBuf[rhs].type == VARIABLE))
        {
            varToNum(lhs);
            varToNum(rhs);
            if (tokenBuf[op].value == '*')
                tokenBuf[lhs].value = tokenBuf[lhs].value * tokenBuf[rhs].value;
            else if (tokenBuf[op].value == '/')
                tokenBuf[lhs].value = tokenBuf[lhs].value / tokenBuf[rhs].value;
            tokenBuf[lhs].type = NUMBER;
            tokenBuf[op].type = EMPTY;
            tokenBuf[rhs].type = EMPTY;
            tokenBuf[op].value = 0;
            tokenBuf[rhs].value = 0;
        }
        else
        {
            lhs = op;
        }
    }
}

void Interpret::doExpr()
{
    char lhs = 0, op, rhs;
    while (true)
    {
        op = this->next(lhs);
        if (op == 0) break;
        rhs = this->next(op);
        if ((tokenBuf[lhs].type == NUMBER || tokenBuf[lhs].type == VARIABLE) &&
                (tokenBuf[op].type == OPERATOR) &&
                (tokenBuf[op].value == '+' || tokenBuf[op].value == '-') &&
                (tokenBuf[rhs].type == NUMBER || tokenBuf[rhs].type == VARIABLE))
        {
            varToNum(lhs);
            varToNum(rhs);
            if (tokenBuf[op].value == '+')
                tokenBuf[lhs].value = tokenBuf[lhs].value + tokenBuf[rhs].value;
            else if (tokenBuf[op].value == '-')
                tokenBuf[lhs].value = tokenBuf[lhs].value - tokenBuf[rhs].value;
            tokenBuf[lhs].type = NUMBER;
            tokenBuf[op].type = EMPTY;
            tokenBuf[rhs].type = EMPTY;
            tokenBuf[op].value = 0;
            tokenBuf[rhs].value = 0;
        }
        else
        {
            lhs = op;
        }
    }
}

void Interpret::doRelation()
{
    char lhs = 0, op, rhs;
    while (true)
    {
        op = this->next(lhs);
        if (op == 0) break;
        rhs = this->next(op);
        if ((tokenBuf[lhs].type == NUMBER || tokenBuf[lhs].type == VARIABLE) &&
                (tokenBuf[op].type == RELOP) &&
                (tokenBuf[op].value == '<' || tokenBuf[op].value == '=' || tokenBuf[op].value == '!' || tokenBuf[op].value == '>') &&
                (tokenBuf[rhs].type == NUMBER || tokenBuf[rhs].type == VARIABLE))
        {
            varToNum(lhs);
            varToNum(rhs);
            if (tokenBuf[op].value == '<')
                tokenBuf[lhs].value = tokenBuf[lhs].value < tokenBuf[rhs].value;
            else if (tokenBuf[op].value == '=')
                tokenBuf[lhs].value = tokenBuf[lhs].value == tokenBuf[rhs].value;
            else if (tokenBuf[op].value == '!')
                tokenBuf[lhs].value = tokenBuf[lhs].value != tokenBuf[rhs].value;
            else if (tokenBuf[op].value == '>')
                tokenBuf[lhs].value = tokenBuf[lhs].value > tokenBuf[rhs].value;
            tokenBuf[lhs].type = NUMBER;
            tokenBuf[op].type = EMPTY;
            tokenBuf[rhs].type = EMPTY;
            tokenBuf[op].value = 0;
            tokenBuf[rhs].value = 0;
        }
        else
        {
            lhs = op;
        }
    }
}

bool Interpret::cmdPrint(char bufferStart)
{
    char whatToPrint = this->next(bufferStart);
    if ((tokenBuf[whatToPrint].type == NUMBER || tokenBuf[whatToPrint].type == VARIABLE)) {
        varToNum(whatToPrint);
        char whatElse = this->next(whatToPrint);
        if ((tokenBuf[whatElse].type == NUMBER || tokenBuf[whatElse].type == VARIABLE)) {
          varToNum(whatElse);
          sprintf(this->msg, "%d%d\n", tokenBuf[whatToPrint].value, tokenBuf[whatElse].value); // Falls String definiert ist, diesen mit ausgeben
        } else if (tokenBuf[whatElse].type == STRING) {
          sprintf(this->msg, "%d%s\n", tokenBuf[whatToPrint].value, tokenizer->stringparam); // Falls String definiert ist, diesen mit ausgeben
        } else {
          sprintf(this->msg, "%d\n", tokenBuf[whatToPrint].value); // Falls String definiert ist, diesen mit ausgeben
        }
    } else if ((tokenBuf[whatToPrint].type == STRING)) {
        char whatElse = this->next(whatToPrint);
        if ((tokenBuf[whatElse].type == NUMBER || tokenBuf[whatElse].type == VARIABLE)) {
          varToNum(whatElse);
          sprintf(this->msg, "%s%d\n", tokenizer->stringparam, tokenBuf[whatElse].value); // Falls String definiert ist, diesen mit ausgeben
        } else {
          sprintf(this->msg, "%s\n", tokenizer->stringparam); // Falls String definiert ist, diesen mit ausgeben
        }
    }
    this->outputAvaiable = true;
    return true;
}

bool Interpret::cmdGoto(char bufferStart)
{
    char whereToGo = this->next(bufferStart);
    if (!(tokenBuf[whereToGo].type == NUMBER || tokenBuf[whereToGo].type == VARIABLE))
        return false; // GOTO zu Variable oder Zahl erlaubt
    varToNum(whereToGo); // Zahl statt dem Variablennamen
    int16_t i = tokenizer->findLineNumber(tokenBuf[whereToGo].value); // Ziel finden
    if (i >= 0)
    {
        this->line = i;
    }
    else
    {
        return false; //Zeilennummer nicht gefunden
    }
    return true;
}

bool Interpret::cmdAsk(char bufferStart)
{
    char whatToAsk = this->next(bufferStart);
    if (!(tokenBuf[whatToAsk].type == VARIABLE))
        return false; // Variable erwartetGOTO zu Variable oder Zahl erlaubt
    requestUserInput = true; // Verarbeitung anhalten bis input aufgerufen
    userInputVariable = tokenBuf[whatToAsk].value;
    if (strlen(tokenizer->stringparam) > 0)
    {
        sprintf(this->msg, "%s\n", tokenizer->stringparam);
    }
    else
    {
        sprintf(this->msg, "Eingabe %c:\n", tokenBuf[whatToAsk].value + 'A');
    }
    return true;
}

bool Interpret::cmdRnd(char bufferStart) {
    char whereToSave = this->next(bufferStart);
    char minRnd = this->next(whereToSave);
    char maxRnd = this->next(minRnd);
    if (!(tokenBuf[whereToSave].type == VARIABLE))
        return false; // Ziel muss Variable sein
    if (!(tokenBuf[minRnd].type == NUMBER || tokenBuf[minRnd].type == VARIABLE))
        return false; // Mindestwert muss Variable oder Zahl sein
    if (!(tokenBuf[maxRnd].type == NUMBER || tokenBuf[maxRnd].type == VARIABLE))
        return false; // Maximalwert muss Variable oder Zahl sein
    varToNum(minRnd); // Zahl statt dem Variablennamen
    varToNum(maxRnd); // Zahl statt dem Variablennamen
#if not defined(__AVR_ATmega328P__)
    // Kein echter Zufall für Testbarkeit
    vars[tokenBuf[whereToSave].value] = (tokenBuf[minRnd].value + tokenBuf[maxRnd].value) / 2;
#else
    vars[tokenBuf[whereToSave].value] = random(tokenBuf[minRnd].value, tokenBuf[maxRnd].value);
#endif
    return true;
}

bool Interpret::cmdFor(char bufferStart) {
    char counter = this->next(bufferStart);
    char startValue = this->next(counter);
    char toSymbol = this->next(startValue);
    char endValue = this->next(toSymbol);
    if (!(tokenBuf[counter].type == VARIABLE))
        return false; // counter muss Variable sein
    if (!(tokenBuf[startValue].type == NUMBER || tokenBuf[startValue].type == VARIABLE))
        return false; // startValue muss Variable oder Zahl sein
    if (!(tokenBuf[endValue].type == NUMBER || tokenBuf[endValue].type == VARIABLE))
        return false; // endValue muss Variable oder Zahl sein
    if (!(tokenBuf[toSymbol].value == TO))
        return false; // TO muss vorhanden sein
    varToNum(startValue); // Zahl statt dem Variablennamen
    varToNum(endValue);
    // Counter initialisieren
    if (forRunning[tokenBuf[counter].value]) {
        if (vars[tokenBuf[counter].value] < tokenBuf[endValue].value) {
            vars[tokenBuf[counter].value]++;
        } else {
            forRunning[tokenBuf[counter].value] = false;
        }
    } else {
        vars[tokenBuf[counter].value] = tokenBuf[startValue].value;
        forRunning[tokenBuf[counter].value] = true;
    }
    forLineVars[tokenBuf[counter].value] = tokenBuf[0].value;
    return true;
}

bool Interpret::cmdNext(char bufferStart) {
    char counter = this->next(bufferStart);
    if (!(tokenBuf[counter].type == VARIABLE))
        return false; // counter muss Variable sein
    if (forRunning[tokenBuf[counter].value]) {
        int16_t i = tokenizer->findLineNumber(forLineVars[tokenBuf[counter].value]); // Ziel finden
        {
            this->line = i;
        }
    }
    return true;
}

bool Interpret::cmdLet(char bufferStart) {
    char whereToSave = this->next(bufferStart);
    char whatToSave = this->next(whereToSave);
    if (!(tokenBuf[whereToSave].type == VARIABLE))
        return false; // Ziel muss Variable sein
    if (!(tokenBuf[whatToSave].type == NUMBER || tokenBuf[whatToSave].type == VARIABLE))
        return false; // Mindestwert muss Variable oder Zahl sein
    varToNum(whatToSave); // Zahl statt dem Variablennamen
    vars[tokenBuf[whereToSave].value] = tokenBuf[whatToSave].value;
    return true;
}

void Interpret::doCommands()
{
    char v1 = 0;
    while ((v1 = this->next(v1)) != 0)
    {
        if (tokenBuf[v1].type == KEYWORD)
        {
            if (tokenBuf[v1].value == PLINE)
            {
                if (!cmdPrint(v1))
                    goto error;
            }
            if (tokenBuf[v1].value == PRINT)
            {
                if (!cmdPrint(v1))
                    goto error;
                // Newline entfernen
                for (int i = 0; i < strlen(msg); i++) {
                    if (msg[i] == '\n')
                        msg[i] = 0;
                }
            }
            else if (tokenBuf[v1].value == GOTO)
            {
                if (!cmdGoto(v1))
                    goto error;
            }
            else if (tokenBuf[v1].value == ASK)
            {
                if (!cmdAsk(v1))
                    goto error;
            }
            else if (tokenBuf[v1].value == RND)
            {
                if (!cmdRnd(v1))
                    goto error;
            }
            else if (tokenBuf[v1].value == LET)
            {
                if (!cmdLet(v1))
                    goto error;
            }
            else if (tokenBuf[v1].value == IF)
            {
                char ifClause = this->next(v1);
                if (tokenBuf[ifClause].value == 0)
                {
                    break;
                }
            }
            else if (tokenBuf[v1].value == FOR)
            {
                if (!cmdFor(v1))
                    goto error;
            }
                        else if (tokenBuf[v1].value == NEXT)
            {
                if (!cmdNext(v1))
                    goto error;
            }
            else if (tokenBuf[v1].value == END)
            {
                this->running = false;
            }
        }
    }
    return;
error:
    sprintf(this->msg, "SYNTAX FEHLER %-d", tokenBuf[0].value);
    outputAvaiable = true;
    this->running = false;
}

void Interpret::varToNum(char pos)
{
    if (tokenBuf[pos].type == VARIABLE)
    {
        tokenBuf[pos].value = vars[tokenBuf[pos].value];
        tokenBuf[pos].type == NUMBER;
    }
}

char* Interpret::output()
{
    outputAvaiable = false;
    return this->msg;
}

void Interpret::input(int16_t in)
{
    if (requestUserInput)
    {
        vars[userInputVariable] = in;
        requestUserInput = false;
    }
}

void Interpret::debugTokenBuf()
{
#if not defined(__AVR_ATmega328P__)
    int16_t i = 0;
    std::cout << "DEBUG:" << std::endl;
    while (this->tokenBuf[i].type != PROGEND)
    {
        std::cout << this->tokenBuf[i].type << '\t' << this->tokenBuf[i].value << std::endl;
        i ++;
    }
    std::cout << this->tokenBuf[i].type << '\t' << this->tokenBuf[i].value << std::endl;

#endif
}

Interpret::~Interpret()
{
    free(tokenBuf);
    free(msg);
}
