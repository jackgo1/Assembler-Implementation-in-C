.extern BN
MAIN: mov @r5 ,K
prn -3
sub @r4, @r6
LOOP2: jmp R6
add @r4, @r7
T1:  .extern   BN 
mcro macro1
    not @r5
    inc R1
endmcro
bne T1
E: not @r2
.entry LOOP2
bne K
add @r2, @r5
R6: inc E
sub @r1, @r4
K: red @r7
mcro macro2
    ;comment line
    add @r3, @r5
endmcro
E2: .entry LOOP2
jmp BN
macro2
END: stop
.string "ijklmnop"
 R1: .string "cdefghi"
.data 6,9,18,24
.data 22
.extern T1