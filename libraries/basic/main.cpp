/* Tests for the Basic Interpreter to be run
 * on a development Platform.
 *
 * @author reineckm
 * @license Apache-2.0
 */

#include "Tokenizer.h"
#include "Interpret.h"
#if not defined(__AVR_ATmega328P__)
#include <string.h>
#include <stdio.h>
#include <iostream>
bool test(Interpret* inter, char* prog, char* result)
{
    //test->debugOut();
    inter->load(prog);
    while(inter->run())
    {
        //std::cout << inter->output() <<  std::endl;
        if (inter->requestUserInput)
        {
            int16_t a;
            std::cout << inter->output() <<  std::endl;
            scanf("%d", &a);
            inter->input(a);
        }
    }
    std::cout <<  std::endl << "[" << inter->output() << "]";
    std::cout << "[" << result << "]" << std::endl;
    //test->debugOut();
    return  (strcmp(inter->output(), result) == 0);
}

int main()
{
    char* t1 = "10 LET A 2\n"
               "20 LET A A * 2\n"
               "30 PRINT A\n"
               "40 IF A < 65 GOTO 20\n \n";
    char* e1 = "128";
    char* t2 = "5 LET B 2\n"
               "10 LET A 2\n"
               "20 LET A A * 2\n"
               "30 PRINT A B\n"
               "40 IF A < 65 GOTO 20\n"
               "50 LET A 3\n"
               "60 LET B B + 1\n"
               "70 IF B < 9 GOTO 20\n";
    char* e2 = "96 8";
    char* t3 = "10 ASK A \"A 14:\"\n"
               "20 ASK B\n"
               "30 IF A = 0 PRINT \"GGT \" B END\n"
               "40 IF B = 0 GOTO 99\n"
               "50 IF A > B GOTO 80\n"
               "60 LET B B - A\n"
               "70 GOTO 40\n"
               "80 LET A A - B\n"
               "90 GOTO 40\n"
               "99 PRINT \"GGT: \" A\n";
    char* e3 = "GGT: 7";
    char* t4 = "10 LET F 2\n"
               "20 LET E F * F\n"
               "30 PRINT \"E zum Quadrat: \" E\n";
    char* e4 = "E zum Quadrat: 4";
    char* t5 = "10 RND F 2\n"
               "20 PRINT F \"RND: \"\n";
    char* e5 = "SYNTAX FEHLER 10";
    char* t6 = "10 RND F 2 4\n"
               "20 PRINT F \" <- RND\"\n";
    char* e6 = "3 <- RND";
    char* t7 = "10 LET A 2\n"
               "20 PRINT A A\n";
    char* e7 = "2 2";
    char* t8 = "10 LET F -2\n"
               "20 LET F F * F\n"
               "30 PRINT \"F: \" F\n"
               "40 IF F < 16 GO 20\n";
    char* e8 = "F: 16";
    char* t9 = "10 FOR A 12 TO 20\n"
               "20 PRINT A\n"
               "30 NEXT A\n";
    char* e9 = "20";
    char* t10 = "10 LET A 10 % 3\n"
                "20 PRINT A\n";
    char* e10 = "1";
    Tokenizer* tok = new Tokenizer(10);
    Interpret* inter = new Interpret(tok);
    std::cout << test(inter, t1, e1) << std::endl;
    std::cout << test(inter, t2, e2) << std::endl;
    std::cout << test(inter, t3, e3) << std::endl;
    std::cout << test(inter, t4, e4) << std::endl;
    std::cout << test(inter, t5, e5) << std::endl;
    std::cout << test(inter, t6, e6) << std::endl;
    std::cout << test(inter, t7, e7) << std::endl;
    std::cout << test(inter, t8, e8) << std::endl;
    std::cout << test(inter, t9, e9) << std::endl;
    std::cout << test(inter, t10, e10) << std::endl;
    std::cout << test(inter, "20 PRINT 0 \"..   . . \"\n", "0..   . . ") << std::endl;
    return 0;
}
#endif
