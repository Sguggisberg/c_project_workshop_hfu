# Battleship Touch BLE Modular v2

Neue Version mit **schönerer Touch-Oberfläche** und **manueller Schiffplatzierung**.

## Highlights
- aufgeteilt in Klassen:
  - `GameController`
  - `BLEManager`
  - `UIManager`
- Touchscreen-Steuerung mit:
  - `ROT H / ROT V` für Ausrichtung
  - `MIX` für Zufallsplatzierung
  - `UNDO` für letztes Schiff entfernen
  - `OK` für READY
  - `RST` für Reset
- Schiffplatzierung direkt auf dem linken Spielfeld
- Angriff direkt auf dem rechten Spielfeld
- automatischer BLE-Rollenwechsel Central / Peripheral

## Bedienung
1. Beide Boards mit dem gleichen Sketch flashen.
2. Auf dem linken Spielfeld die Schiffe platzieren.
3. Mit `ROT` zwischen horizontal und vertikal wechseln.
4. Mit `MIX` zufällig setzen oder mit `UNDO` zurück.
5. Wenn alle Schiffe platziert sind, `OK` drücken.
6. Sobald beide Spieler bereit sind, beginnt das Spiel.
7. Das Central-Board startet.

## Benötigte Bibliotheken
- `ArduinoBLE`
- `Adafruit GFX Library`
- `Adafruit ILI9341`
- `XPT2046_Touchscreen`

## Hardware
Ausgelegt für **Arduino Nano RP2040 Connect** mit ILI9341 + XPT2046 Touch.

## Pins
Standardwerte in `Config.h`:
- `TFT_CS = 10`
- `TFT_DC = 9`
- `TFT_RST = 8`
- `TOUCH_CS = 7`
- `TOUCH_IRQ = 6`

## Touch-Kalibrierung
Bei versetzter Eingabe diese Werte in `Config.h` anpassen:
- `TOUCH_MIN_X`
- `TOUCH_MAX_X`
- `TOUCH_MIN_Y`
- `TOUCH_MAX_Y`

## Nächste sinnvolle Erweiterungen
- Vorschau des aktuell zu platzierenden Schiffs vor dem Tippen
- Splashscreen / Animationen
- komplette Versenkt-Markierung des gegnerischen Schiffs
- Soundeffekte


Version v3:
- Display-Flackern reduziert durch partielles Redraw statt kompletter Screen-Neuzeichnung in jedem Refresh.
- UI-Refresh auf 120 ms entschärft.


Update v6:
- Bildschirmzeichnen in getrennte Screens aufgeteilt
- Eigener Wartebildschirm fuer "Peripheral wartet"
- Automatischer Wechsel vom Wartebildschirm zur Spielansicht nach Verbindung
