.entry L1
.extern W
MAIN: mov @r4 , T
  LOOP:     cmp -4 -3
bne ENDLOOP
mcro macro1
   add @r3, @r4
   sub @r5, @r6
endmcro
add @r4  , R
lea ET , Y
W: add @r3, @r7  
jmp , LOOP 
START: sub , 
T: sub @r2, @r4
clr STR
RB , , 
@r4
   not @r3 , 4
L: sub @r1, @r7
B
bne MAIN
B: .data 76
STR: .string "abcde
ARR: .data 12,,6,4,7,,2
K: .data 26, 10
A: .data 
.string abcd