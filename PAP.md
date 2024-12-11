## Ablaufplan Messgerät-Server
```mermaid
flowchart TD
    id0([Deno Beginnt])
    id0 --> id1[Server startet mit Deno.serve auf dem Port 8000]
    id1 --> id2[Eingehende URLs speichern und an jedem '/' teilen]
    id2 --> id3{Entscheiden, ob die eingehende URL einen neu verbundenen ESP ankündigt, Messdaten darstellt oder Müll ist}

    id3 --Link kündigt neuen ESP an---id4A(Derzeitiges Datum und Uhrzeit abrufen)
    id4A --> id4B(ESP-ID aus dem Link schließen)
    id4B --> id4C(Neue CSV-Datei mit dem ID und Datum im Namen sowie in der ersten Zeile Zeit, Puls und Widerstand erstellen)
    id4C --> id4D[/ESP-ID zur Konsole ausgeben/]
    id4D --> id4E[/Tabellen-Namen in Registerdatei schreiben/]

    id3 --Link schickt Daten---id5A(Daten aus dem Link schließen)
    id5A --> id5B(Mithilfe der ID und des Registers die richtige Tabelle finden)
    id5B --> id5C[/Daten in die richtige Tabelle eintragen/]

    id3 --Link entspricht weder Datenformat noch Verbindungsformat---id6A[/Falschen Link zur Konsole ausgeben/]
    id6A --> id6B[/404 als antwort auf den HTTP-request schicken/]

    id5C --> id7
    id4E --> id7
    id7[/OK als antwort auf alle Richtigen HTTP-Requests/]
```

## Ablaufplan ESP-Code
```mermaid
flowchart TD
    id0([Start])
    id0 --> id1[Variablen initialisiern und Konstanten definieren]
    id1 --> id2[["setup() beginnt"]]
    id2 --> id3[Warten, Bis mit WLAN verbunden wurde]
    id3 --> id4[Mit dem Server verbinden, ID senden]
    id4 --> id5[["setup() endet"]]
    id5 --> id6[["anfang loop(); loop wird hier durch das Delay am Ende alle 20ms ausgeführt"]]
    id6 --> id7[Derzeitige ESP-laufzeit speichern]
    id7 --> id8[Messung von dem Herzfrequenz-Messgerät in eine Reihe speichern]
    id8 --> id9[Alle 500s Herzfrequenzgerät-Messreihe auswerten: Herzschläge suchen, BPM berechnen & speichern] 
    id9 --> id10[Hautwiderstand messen, berechnen & speichern]
    id10 --> id11[/Jetzige Zeit, ESP-ID und Messwerte an den Server senden/]
    id11 --> id12[20ms warten]
    id12 --> id13[["ende loop()"]]