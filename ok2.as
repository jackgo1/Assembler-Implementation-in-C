.entry LIST
.extern W
MAIN: add @r5, @r2
LOOP: prn 10
mcro macro1
 inc @r3
 mov @r4, W
endmcro
GH: sub @r3, @r7
mcro macro2
    add @r6, @r3
    ;comment line 
    lea STR, @r6
endmcro
mcro macro3
    ;comment line
    ;comment line
    ;comment line
endmcro
STR: .string "abcde"
LIST: .data 6, -9
inc @r4
mov @r5, W
sub @r1, @r6
macro2
bne END
cmp THJ, -6
macro3
bne END
dec K
.entry MAIN
sub LOOP , @r5
END: stop
.entry K
macro1
K: .data 21
.extern THJ
macro1
