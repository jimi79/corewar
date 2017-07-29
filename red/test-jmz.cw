JMZ #0,2 ; should skip next DAT
DAT #0,#0
JMZ 1,2 ; should skip next DAT
DAT #0,#0
JMZ @1,3 ; should skip next 2 DAT
DAT #0,#2
DAT #0,#0
JMZ #1,2 ; shouldn't skip next DAT
DAT #0,0
MOV 0,1 
