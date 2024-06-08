# MiniShell
Bedienung auf Linux

$ pwd                  # Wir sind noch in der Unix-Shell
/tmp

$ ~/bs/Aufg1/miniShell # Aufruf Ihrer Minishell

/tmp > date
Wed Oct 19 03:40:54 PM CEST 2022

/tmp > sleep 20

/tmp > date
Wed Oct 19 03:41:30 PM CEST 2022

/tmp > sleep 20 &
[97160]

/tmp > date
Wed Oct 19 03:41:34 PM CEST 2022

/tmp > ps
PID TT STAT TIME COMMAND
96440 s000 S 0:00.04 -bash
97144 s000 S+ 0:00.00 miniShell
97160 s000 S+ 0:00.00 sleep 20

Process 97160 done (sleep 20)

/tmp > ps
PID TT STAT TIME COMMAND
96440 s000 S 0:00.04 -bash
97144 s000 S+ 0:00.00 miniShell

/tmp > ^C  
Do you really want to quit (y/n) ? y

$ echo $SHELL          # Wieder in normaler Shell     
