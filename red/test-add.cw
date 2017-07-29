ADD #1,10 ; add 1 to the first dat
ADD 10,9  ; add the value of dat #2 to the first DAT
ADD @10,8 ; add the value of the last DAT to the first DAT
ADD 10,@5 ; add the value 10 to the same DAT again
DAT #0,#0
DAT #0,#0
DAT #0,#0
DAT #0,#0
DAT #0,#7 ; point to the DAT two steps further
DAT #0,#0
DAT #0,#100 ; will have 1, then 3, then 13
DAT #0,#2
DAT #0,#11 ; will take the 10
DAT #0,#10
