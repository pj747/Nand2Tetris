// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

//Resets to return the counter at the end of clearing and end of darkening
(BEGINNING)
@SCREEN
D = A
@i
M = D
@CHECK
0; JMP

(END)
@24576
D = A
@i
M = D
@CHECK
0; JMP

(CHECK)
@KBD
D = M
@CLEAR
D; JEQ
@FILL
D; JNE

//KEY PRESSED
(FILL)
@i
D = M
@24575
D = D - A
@END
D; JGT
@i 
A = M
M = -1
@i
M = M + 1
@i
D = M
@CHECK
0;JMP

(CLEAR)
@i
D = M
@SCREEN
D = D - A
@BEGINNING
D; JLT
@i
A = M
M = 0
@i
M = M - 1
@CHECK
0;JMP


