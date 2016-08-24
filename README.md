# ArduinoBasic

This is code for an oldshool Basic Homecomputer designed to fit into a old PS/2 Keyboard. The computer puts out a 29 by 9 characters Black and White AV signal thanks to the incredible [TVOut Libary|https://github.com/Avamander/arduino-tvout]. Hardwarewise you only need some kind of preffebly small Arduino like a Arduino Pro, 3 Resistors, 2 RCA Jacks and a PS/2 Keyboard.

# Basic

The Computer boots directly to a pretty barebone Texteditor where you can code your Basic. That Basic features as of now
* Basic Integer Arithmetic +, -, *, /
* Relational Operators <, >, =
* The following Keyword  LET, RND, PRINT, PLINE, ASK, GOTO, IF, ASK, END, FOR, NEXT, TO
* Single digit (16bit signed) Integer Variable
* Strings in kconjunktion with Print and pline

Since the used MCU has only 2KB of Ram your Programms can't be longer than 400 chars atm. Increasing this Number will be the next step after refactoring the working code so far.
