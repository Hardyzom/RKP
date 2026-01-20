# chart - Rendszerközeli programozás projekt

Egyszerű mérésküldő és -fogadó alkalmazás, amely két üzemmódban futtatható (küldő / fogadó), és kétféle kommunikációt támogat: **fájlon** vagy **socketen** keresztül. A fogadó fél a beérkező mérésekből egy `chart.bmp` képet generál, amely grafikonként jeleníti meg az adatokat.

---

## Rendszerkövetelmények (példa fejlesztői környezet)
- 4 GB RAM
- AMD FX™-8350 (8 mag), NVIDIA GTX 1650
- Ubuntu 22.04.3 LTS (64-bit)
- GCC 11.4.0

---

## Fordítás

```bash
# OpenMP-vel (ajánlott, ha használod a párhuzamosítást)
gcc -O2 -fopenmp -o chart chart.c

# OpenMP nélkül
gcc -O2 -o chart chart.c
```

A sikeres fordítás után a futtatható állomány neve: `chart`.

---

## Használat

### Alap parancsok
```bash
./chart               # Alapértelmezett futás (ha így van beállítva a kódban)
./chart --help        # Lehetséges kapcsolók listája
./chart --version     # Verzió, build dátum, fejlesztő neve
```

### Üzemmódok és kommunikációs módok

**Fájl alapú kommunikáció**
```bash
./chart -file -send      # Küldő üzemmód fájlon keresztül
./chart -file -receive   # Fogadó üzemmód fájlon keresztül
```

**Socket alapú kommunikáció**
```bash
./chart -socket -send      # Küldő üzemmód socketen
./chart -socket -receive   # Fogadó üzemmód socketen
```

> A kapcsolók **tetszőleges sorrendben** kombinálhatók (pl. `-send -socket` is érvényes).

### Kimenet

- Fogadó üzemmódban a program a beérkező adatokból **`chart.bmp`** képfájlt készít (grafikon).

---

## Visszatérési kódok (exit code-ok)

| Kód | Jelentés |
|-----|----------|
|  0  | Sikeres futás |
|  1  | Hibás argumentum-név |
|  2  | Hibás fájlnév (nem `./chart`) |
|  3  | Hiba a `/proc` mappa megnyitásakor |
|  4  | Hiba a `/proc/status` megnyitásakor |
|  5  | Hiba a `statuscopy.txt` megnyitásakor |
|  6  | Nincs fogadó üzemmódban lévő process |
|  7  | Memóriafoglalási hiba (dinamikus tömb) |
|  8  | Nem található a `~/Measurement.txt` |
|  9  | Socket létrehozási hiba |
|  10 | Küldési hiba |
|  11 | Fogadási hiba |
|  12 | Nem egyezik a visszaküldött bájt-méret |
|  13 | Nem egyezik a visszaküldött tömbméret |
|  14 | A szerver leállt |
|  15 | Hozzárendelési hiba |
|  16 | `SIGINT`: elköszönő üzenet és kilépés |
|  17 | `SIGALRM`: a szerver nem válaszol időben |

---

## Főbb komponensek (röviden)

- **`help()`** - a futtatási argumentumok listája.
- **`Measurement(int** Values)`** - dinamikus tömböt foglal és tölti fel mérési adatokkal; visszatérési értéke az elemek száma (min. 100, másodperc-felbontás negyedórán belül). Az értékek lépésenként nőhetnek, csökkenhetnek vagy maradhatnak.
- **`hatvany(int alap, int kitevo)`** - pozitív egész kitevőjű hatvány iteratív számítása.
- **`BMPformat(int x, char tomb[])`** - 32 bites egész konvertálása 4 bájtos karaktertömbre (little-endian).
- **`BMPcreator(int width, char buffer[])`** - BMP fájl összeállítása és kiírása a mért adatok vizualizálására (`chart.bmp`).
- **`FindPID()`** - megkeres egy adott nevű folyamatot Linuxon a `/proc` alatt (pl. `chart`), visszaadja a PID-et.
- **Dinamikus tömbkezelés** - `memory_err()`, `dt_create()`, `dt_add()`, `dt_destroy()`; létrehozás, bővítés (`realloc`), felszabadítás.
- **`SendViaFile(int *Values, int NumValues)`** - `Measurement.txt` fájlba ír, majd `SIGUSR1` jelet küld a fogadónak.
- **`ReceiveViaFile(int sig)`** - fájlból olvas `SIGUSR1` hatására, különbségeket számol, BMP-t generál, memóriát felszabadít.
- **`SendViaSocket(int *Values, int NumValues)`** - kliens oldali küldés socketen; ellenőrzések és lezárás.
- **`ReceiveViaSocket()`** - szerver/fogadó: adatok várása, feldolgozása, visszajelzés.

---

## Gyors példák

### 1) Fájl alapú kommunikáció
```bash
# Terminál A (fogadó):
./chart -file -receive

# Terminál B (küldő):
./chart -file -send

# Eredmény: chart.bmp a fogadó oldalon
```

### 2) Socket alapú kommunikáció
```bash
# Terminál A (fogadó/szerver):
./chart -socket -receive

# Terminál B (küldő/kliens):
./chart -socket -send
```

---

## Tesztelési tippek

- Használj kis, majd növekvő elemszámú mintát a `Measurement()` ellenőrzésére.
- Ellenőrizd a BMP fejlécet (`file chart.bmp` vagy hex viewer), ha a kép nem nyílik meg.
- Időzítéses hibákhoz érdemes `SIGALRM`-mal timeoutot beállítani (szerver oldalon).

---

## Hibaelhárítás

- **„Nincs fogadó üzemmód”** - indítsd el előbb a fogadót (`-receive`), majd a küldőt.
- **„Measurement.txt nem található”** - ellenőrizd az elérési utat (pl. `~/Measurement.txt`) és a jogosultságokat.
- **Socket hibák (9–13)** - ellenőrizd a tűzfalat/portot, a szerver elérhetőségét és hogy a fogadó tényleg fut-e.
- **Üres vagy sérült `chart.bmp`** - nézd meg, jönnek-e valós adatok (mintaadatokkal is tesztelhetsz), illetve nem fogyott-e el a memória.
