# MiniShell

Die MiniShell ist ein einfacher Kommando-Interpreter, der in C++ implementiert ist und sich an der Unix-Shell orientiert. Sie ermöglicht das Ausführen von Befehlen, das Erstellen von Prozessen sowie das Verwalten von Hintergrundprozessen.

## Funktionalitäten

Die MiniShell bietet folgende Funktionen:

1. **Befehlseingabe**: Der Benutzer kann Befehle eingeben, die von der Shell ausgeführt werden sollen.
2. **Prozesserzeugung**: Die Shell erzeugt einen neuen Prozess für jeden eingegebenen Befehl.
3. **Hintergrundausführung**: Der Benutzer kann einen Befehl mit dem Zeichen '&' am Ende eingeben, um ihn im Hintergrund auszuführen, während er weitere Befehle eingibt.
4. **Signalbehandlung**: Die Shell reagiert auf das SIGINT-Signal (Strg+C), um die Sitzung zu beenden, und das SIGCHLD-Signal, um beendete Hintergrundprozesse zu behandeln.
5. **Beenden der Shell**: Der Benutzer kann die Shell entweder durch Eingabe des Befehls "exit" oder durch das Drücken von Strg+C beenden.

## Beispielverwendung

```sh
$ date
Wed Oct 19 03:40:54 PM CEST 2022

$ sleep 20

$ date
Wed Oct 19 03:41:30 PM CEST 2022

$ sleep 20 &
[97160]

$ date
Wed Oct 19 03:41:34 PM CEST 2022

$ ps
PID TT STAT TIME COMMAND
96440 s000 S 0:00.04 -bash
97144 s000 S+ 0:00.00 miniShell
97160 s000 S+ 0:00.00 sleep 20

Process 97160 done (sleep 20)

$ ps
PID TT STAT TIME COMMAND
96440 s000 S 0:00.04 -bash
97144 s000 S+ 0:00.00 miniShell

$ ^C  
Do you really want to quit (y/n)? y
