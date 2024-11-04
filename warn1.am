.extern K
MAIN: mov @r3 ,K
prn -5
bne K
sub @r4, @r6
LOOP: jmp R6
bne R
.entry LOOP
add @r2, @r5
R6: inc E
sub @r1, @r4
GR:  .entry LOOP
jmp K
LBL:  .extern   K 
END: stop
.string "abcdef"
.data 3,-8,17,22
E: .data 31
.extern R