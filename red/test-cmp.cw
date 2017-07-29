CMP #3,#2 ; will skip
DAT #0,#0
CMP #3,#3 ; will not skip
CMP 2,3 ; will not skip
JMP 3 ; skip DAT
DAT #0,#5
DAT #0,#5
CMP @2,@3  ; will not skip
JMP 6 ; will go to #99
DAT #0,#5
DAT #0,#6
DAT #0,#0
DAT #0,#9
DAT #0,#9
DAT #0,#99
