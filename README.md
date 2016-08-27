# ArduinoBasic

This is code for an oldshool Basic Homecomputer designed to fit into a old PS/2 Keyboard. The computer puts out a 29 by 9 characters Black and White AV signal thanks to the incredible [TVOut Libary](https://github.com/Avamander/arduino-tvout). Hardwarewise you only need some kind of, preferably small, Arduino like a Arduino Pro, 3 Resistors, 2 RCA Jacks and a PS/2 Keyboard.

# Basic

The Computer boots directly to a pretty barebone Texteditor where you can code your Basic. That Basic features as of now
* Integer Arithmetic +, -, *, /
* Relational Operators <, >, =
* The following Keyword  LET, RND, PRINT, PLINE, ASK, GOTO, IF, ASK, END, FOR, NEXT, TO
* Single digit (16bit signed) Integer sariables
* Strings in conjunktion with PRINT and PLINE

Since the used MCU has only 2KB of Ram your Programms can't be longer than 400 chars atm. Increasing this Number will be the next step after refactoring the working code so far.

# Change of Plans...
Since one Arduino can't handle the PS/2 Communication and Video Signal creation very wll together (Maybe one can try to do this only in VBI oder HBI but...) The new way is having 2 Arduinos talking via i2c. One Arduino is the "Graphic-Chip" wich creates the Signal, while the other one does all the Basic stuff. The Graphic chip is the I2C Master, pulling Request whenever he has the time. The "Basic Chip" has a Queue for one Request (TODO: Make it longer) and has to wait when the "Graphic-Chip" is too slow.