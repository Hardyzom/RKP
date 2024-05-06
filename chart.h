#ifndef CHART_H_
#define CHART_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>
#include <dirent.h>
#include <omp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define VERSION 1.0                  // A program verzió száma
#define DATE "2024.05.02"            // A program elkészülésének dátuma
#define AUTHOR "Pasch Alex Karoly"   // A készítő neve

int s;                               // socket ID

// 7. feladat
void SignalHandler(int sig)
{
    if(sig == SIGINT)
    {
        printf("A viszont latatra Kedves!\n");
        exit(16);
    }
    else if(sig == SIGUSR1)
    {
        fprintf(stderr, "A fájlon keresztüli küldés szolgáltatás nem elérhető!\n");
    }
    else if(sig == SIGALRM)
    {
        fprintf(stderr, "A szerver nem válaszol (időkereten belül)\n");
        exit(17);
    }
}

// 1. feladat
void help()
{
    printf("Lehetséges futtatási argumentumok:\n");
    printf("\t--help - a futtatás lehetseges opcioi\n");
    printf("\t--version - az aktualis verzio infoi\n");
    printf("\t-send - kuldo modba allitas (ez az alapertelmezett mod)\n");
    printf("\t-receive - fogado modba allitas\n");
    printf("\t-file - a kommunikacio modja file lesz (ez az alapertelmezett mod)\n");
    printf("\t-socket - a kommunikacio modja socket lesz\n");
}


// 2. feladat 1. lépés
int Measurement(int** Values)
{
    srand(time(0)); // Inicializálja a véletlen számgenerátort az aktuális idő alapján
    time_t time1; // Deklarál egy time_t típusú változót az idő tárolására
    int time2 = time(&time1); // Beállítja a time2 változót az aktuális időhöz (UNIX idő, másodpercek 1970-től)
    int meret = time2 % 900;
    if(meret < 100)
    {
        meret = 100;
    }
    int* mutato = malloc(meret * sizeof(int)); // Foglal memóriát 'meret' darab int méretére

    mutato[0] = 0; // Az első elemet 0-ra inicializálja

    for (int i = 1; i < meret; i++)
    {
        
        
        int min = 1;
        int max = 1000000; // 100%
        int randomszam = rand() % ((max+1) - min) + min;

        if(randomszam <= 428571) // 42,8571% - +1
        {
            mutato[i] = mutato[i-1] + 1; // Növeli az előző elemet 1-gyel
        }
        else if(randomszam <= 783410) //42,8571 + 35,4839 = 78,3410 - -1
        {
            mutato[i] = mutato[i-1] - 1;
        }
        else
        {
            mutato[i] = mutato[i-1]; // 21,6590% - 0 Ez pedig a maradék a 100%-ból
        }
    }

    *Values = mutato; // A pointerbe beállítja a létrehozott tömb pointerét

    return meret;
}



// 3.feladat
int hatvany(int alap, int kitevo)
{
    int eredmeny = 1;
    for(int i = 0; i < kitevo; i++) //Addig megy, amíg el nem éri a kitevő értékét
    {
        eredmeny *= alap;
    }

    return eredmeny; //Visszaadja az eredményt, ami az alap kitevőre emelve
}

void BMPformat(int x, char tomb[])
{
    int szamtomb[] = {0, 0, 0, 0};
    int szamlalo = 31;
    int elem = 3; // beállítunk egy változót az eredménytömb utolsó indexére
    unsigned int y = 1<<(sizeof(int)*8-1); //beállítja a legmagasabb bitet

    while(y)
    {
        if((x & y)!=0)
        {
            szamtomb[elem] = szamtomb[elem] | (hatvany(2, (szamlalo % 8 ))); // Ha a bit 1, beállítja a megfelelő bitet a vissza tömb elemében
        }

        y = y >> 1; // Jobbra tolja a y bitjét, azaz csökkenti a vizsgált bit pozícióját
        szamlalo = szamlalo-1;
        if((szamlalo + 1) % 8 == 0) //Minden 8. lépésnél (magyarul új bájt esetében) csökkenti az elem változót, hogy a következő bájt bitjeit állítsa be
        {
            elem = elem - 1;
        }
    }
    for(int i = 0; i < 4; i++)
    {
        tomb[i] = szamtomb[i]; // A kimeneti tömb megkapja a vissza tömb értékeit
    }
}

void BMPcreator(int *Values, int NumValues)
{
    int meret;
    if (NumValues % 32 == 0)
    {
        meret = NumValues;
    }
    else
    {
        meret = ((NumValues / 32) + 1) * 32;
    }

    int maxmeret = 62 + ((meret/8) * NumValues); // ez számolja ki a BMP fájl teljes méretét

    int f;
    char* tomb;
    tomb = calloc(maxmeret, sizeof(char)); // Memóriát foglal a BMP fájl adatoknak

    // Beállítja a BMP fejléc bizonyos bájtjait előre meghatározott értékekre
    tomb[0] = 66; tomb[1] = 77; // BM azonosító
    tomb[10] = 62; tomb[14] = 40; // Fix fejléc méret és offset
    tomb[26] = 1; tomb[28] = 1; // Színsíkok és pixelenkénti bitek
    tomb[38] = 97; tomb[39] = 15; // Vízszintes felbontás
    tomb[42] = 97; tomb[43] = 15; // Függőleges felbontás
    tomb[54] = 0; tomb[55] = 0; tomb[56] = 0; tomb[57] = 255; // grafikon háttérszíne fekete tomb: 54 55 56
    tomb[58] = 255; tomb[59] = 255; tomb[60] = 255; tomb[61] = 255; // a fehér színű vonal kirajzolásához a grafikonon


    // beállítja a fájl méretét a ejlécében
    char tombmeret[] = {0, 0, 0, 0};
    BMPformat(maxmeret, tombmeret);
    tomb[2] = tombmeret[0]; tomb[3] = tombmeret[1]; tomb[4] = tombmeret[2]; tomb[5] = tombmeret[3];

    // BMP szélessége
    char tombszelesseg[] = {0, 0, 0, 0};
    BMPformat(NumValues, tombszelesseg);
    tomb[18] = tombszelesseg[0]; tomb[19] = tombszelesseg[1]; tomb[20] = tombszelesseg[2]; tomb[21] = tombszelesseg[3];

    // BMP magassága
    char tombmagassag[] = {0, 0, 0, 0};
    BMPformat(NumValues, tombmagassag);
    tomb[22] = tombmagassag[0]; tomb[23] = tombmagassag[1]; tomb[24] = tombmagassag[2]; tomb[25] = tombmagassag[3];

    tomb[62 + (NumValues/2) * (meret/8)] = tomb[62 + (NumValues/2) * (meret/8)] | (char)(hatvany(2, 7)); //Kezdő pont (origó)

    int jelenlegimag= NumValues / 2; // Kezdő függőleges pozíció a grafikon közepén
    for(int i = 0; i < NumValues; i++)
    {
        int x = i / 8 ; //8 db után eltolja 1B -tal a pixelt
       jelenlegimag += Values[i]; // Frissíti a függőleges pozíciót
       if(jelenlegimag > NumValues - 1){  // Korlátozza az értéket a maximális magasságra
           tomb[62 + x + (NumValues - 1) * (meret/8)] = tomb[62 + x + (NumValues - 1) * (meret/8)] | (char)(hatvany(2, 7 -(i % 8)));
       }
       else{ // Korlátozza az értéket a minimális magasságra
           if(jelenlegimag < 0){
               tomb[62 + x + 0 * (meret/8)] = tomb[62 + x + 0 * (meret/8)] | (char)(hatvany(2, 7 -(i % 8)));
           }
           else{
               tomb[62 + x + (jelenlegimag) * (meret/8)] = tomb[62 + x + (jelenlegimag) * (meret/8)] | (char)(hatvany(2, 7 -(i % 8)));
           }
       }
    }

    f = open("chart.bmp", O_CREAT | O_RDWR , S_IRUSR | S_IWUSR | S_IXUSR);

    write(f,tomb,maxmeret); // Kiírja a BMP adatokat a fájlba
    close(f);
}


// 4. feladat
int FindPID()
{
    FILE *f, *t, *fp;
    DIR *d = opendir("/proc");
    struct dirent *drent;

    if (d == NULL)
    {
        fprintf(stderr, "Hiba a /proc megnyitasaban");
        exit(3);
    }
    
    int pid = -1;
    while (drent = readdir(d)) // Bejárja a /proc könyvtár minden elemét
    {
        if (!isdigit(*drent->d_name)) // Ha nem szám a könyvtár neve (nem PID), akkor kihagyja
        {
            continue;
        }

        chdir("/proc");
        chdir((*drent).d_name); // Belép az aktuális folyamat könyvtárába

        t = fopen("status", "r");

        if (!t)
        {
            fprintf(stderr, "Hiba a /proc/status megnyitasaban");
            exit(4);
        }
        f = fopen("/home/paleksz/statuscopy.txt", "w");
        if (!t)
        {
            fprintf(stderr,"Hiba a statuscopy.txt megnyitasaban");
            exit(5);
        }

        char c;
        while ((c = getc(t)) != EOF) // A status fájl tartalmát átmásolásja az új fájlba
        {
            putc(c, f);
        }
        fclose(f);
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        int bash = 0;

        fp = fopen("/home/paleksz/statuscopy.txt", "r");
        while ((read = getline(&line, &len, fp)) != -1) // Végigolvassa a másolt fájlt
        {

            if (strstr(line, "Name:\tchart")) // Ha megtalálja a "chart" nevű folyamatot, beállít egy jelzőt
            {
                bash = 1;
            }
            if (bash == 1 && strstr(line, "Pid:\t")) // Ha a jelző be van állítva és talál PID sort
            {

                char stringpid[strlen(line) - 5]; // Létrehoz egy stringet a PID tárolására

                for (int i = 5; i < strlen(line); i++) // Kinyeri a PID-et a sorból
                {
                    stringpid[i - 5] = line[i]; // Lezárja a stringet
                }
                if (pid == -1) // Ha még nincs PID tárolva, tárolja el az aktuálisat
                {
                    pid = atoi(stringpid);
                }
            }
        }
        fclose(fp);
        free(line); // Felszabadítja a getline által lefoglalt memóriát
    }
    remove("/home/paleksz/statuscopy.txt");
    fclose(t);
    closedir(d); // Bezárja a /proc könyvtárat
    if (pid == getpid()) // Ha a megtalált PID a saját folyamat PID-je, -1-et ad vissza
    {
        return -1;
    }

    return pid;
}



// 5. feladat 2. lépése
void SendViaFile(int *Values, int NumValues)
{
    FILE* f = fopen("/home/paleksz/Measurement.txt", "w");
    for (int i = 0; i < NumValues; i++)
    {
        fprintf(f,"%d\n", Values[i]); // Kiírja az aktuális értéket a fájlba
    }
    fclose(f);

    int pid = FindPID(); // Megkeresi a "chart" nevű folyamat PID-jét

    if(pid == -1) // Ha nem találja a folyamatot
    {
        fprintf(stderr, "Nem talalok fogado uzemmodban levo processt");
        exit(6);
    }
    else
    {
        printf("Meresek elkuldve\n");
        kill(pid,SIGUSR1); // Elindítja a folyamatot a SIGUSR1 jelzéssel
    }
}

typedef struct
{
    int *elemek;
    int length;
    int capacity;
} Dtomb;

void memory_err()
{
    fprintf(stderr, "Nem sikerült elegendő memóriát foglalni a din. tömbhöz!");
    exit(7);
}

Dtomb *dt_create(Dtomb *self)
{
    self->length = 0;
    self->capacity = 2; // Beállítja a kapacitást

    self->elemek = malloc(self->capacity * sizeof(int)); // Dinamikus memóriaterületet foglal az elemeknek
    if(self->elemek == NULL) // Ellenőrzi, hogy sikerült-e a memória foglalás
    {
        memory_err();
    }

    return self; // Visszatér az inicializált struktúrával
}

void *dt_add(Dtomb *self, int szam)
{
    if(self->capacity == self->length) // Ha a kapacitás megegyezik a hosszal akkor duplázza a kapacitást és újraallokálja a memóriát
    {
        self->capacity *= 2;

        self->elemek = realloc(self->elemek, self->capacity * sizeof(int));
    }

    self->elemek[self->length] = szam; // Hozzáadja az elemet a tömbhöz
    self->length++; // Növeli az elemek számát
}

void *dt_destroy(Dtomb *self)
{
    free(self->elemek); // Felszabadítja az elemek tömbjét
    free(self); // Felszabadítja a Dtomb struktúrát

    return NULL;
}

// 5. feladat 3. lépés
void ReceiveViaFile(int sig)
{
    Dtomb *meresek = malloc(sizeof(Dtomb)); // Létrehozza a meresek nevű dinamikus tömböt
    if(meresek == NULL) // Ellenőrzi, hogy sikerült-e létrehozni a tömböt
    {
        memory_err();
    }
    meresek = dt_create(meresek); // Inicializálja a meresek tömböt
    int meret = 0;
    FILE* fp;
    char* line = NULL; // Sor olvasására használt pointer deklarálása
    size_t len = 0; // Sor hosszának tárolására használt változó deklarálása
    ssize_t read; // Olvasott sor hosszának tárolására használt változó deklarálása

    fp = fopen("/home/paleksz/Measurement.txt", "r");
    if (fp == NULL)
        {
            fprintf(stderr, "Nem találom a /home/paleksz/Measurement.txt-t");
            exit(10);
        }

    while ((read = getline(&line, &len, fp)) != -1) { // A fájl sorainak olvasása
        dt_add(meresek, atoi(line)); // Az olvasott szám hozzáadása a meresek tömbhöz
        meret++;
    }
    meret--; // Az utolsó üres sor miatt csökkenti a méretet
    fclose(fp);
    if (line)
    {
        free(line);
    }

    int meresek_kulonbsegei[meret]; // Tömb deklarálása a mért értékek különbségeinek tárolására
    meresek_kulonbsegei[0] = 0; // Az első különbség mindig 0, mert nincs előző érték
    for (int i = 0; i < meret-2; i++)
    {
        if(meresek->elemek[i+1] > meresek->elemek[i]) // Ha az aktuális érték nagyobb az előzőnél 1-es értéket ad a különbségnek
        {
            meresek_kulonbsegei[i+1] = 1;
        }
        else if(meresek->elemek[i+1] < meresek->elemek[i]) // Ha az aktuális érték kisebb az előzőnél -1-es értéket ad
        {
            meresek_kulonbsegei[i+1] = -1;
        }
        else if(meresek->elemek[i+1] == meresek->elemek[i]) // Ha megegyezik akkor 0-ás értéket ad
        {
            meresek_kulonbsegei[i+1] = 0;
        }
    }

    BMPcreator(meresek_kulonbsegei, meret); // BMP fájl létrehozása a mért értékek különbségeivel
    dt_destroy(meresek); // Felszabadítja a meresek tömb által foglalt memóriát
    printf("Sikeresen le van generalva a bmp\n");
}




// 6. feladat 1. lépése
void SendViaSocket(int *Values, int NumValues)
{
    int tomb[NumValues]; // Lokális tömb létrehozása a kapott értékek mentésére
    for (int i = 0; i < NumValues; i++) // Végigmegy a kapott értékeken és átmásolja őket a lokális tömbbe
    {
        tomb[i] = Values[i];
    }

    
    int s;                            // socket ID
    int bytes;                        // fogadott/küldött bájtok száma
    int flag;                         // átviteli zászló
    char on;                          // sockopt opció
    int buffer;                       // puffertároló
    unsigned int server_size;         // a sockaddr_in server hossza
    struct sockaddr_in server;        // a szerver címe

    


    on   = 1;
    flag = 0;
    server.sin_family      = AF_INET;   // IP verzió
    server.sin_addr.s_addr = inet_addr("10.0.2.15"); // a szerver IP címe
    server.sin_port        = htons(3333); // a szerver portszáma
    server_size = sizeof server; // a szerver struktúra mérete

    
    

    s = socket(AF_INET, SOCK_DGRAM, 0 ); // UDP socket létrehozása
    if ( s < 0 ) { // hibaellenőrzés: ha a socket létrehozása nem sikerült
        fprintf(stderr, "Socket létrehozási hiba\n");
        exit(9);
        }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on); // socket opciók beállítása
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    
    


    bytes = sendto(s, &NumValues, sizeof(int), flag, (struct sockaddr *) &server, server_size); // adatok küldése a szervernek
    if ( bytes <= 0 ) { // hibaellenőrzés: ha a küldés nem sikerült
        fprintf(stderr, "Küldési hiba\n");
        exit(10);
        }
    printf (" %i bájt elküldve a szervernek.\n", bytes-1); // küldött bájtok számának kiírása

    alarm(1); // időzítő beállítása
    signal(SIGALRM,SignalHandler); // időzítő jelzése beállítása

    
    


    bytes = recvfrom(s, &buffer, sizeof(int), flag, (struct sockaddr *) &server, &server_size); // adatok fogadása a szervertől
    if ( bytes < 0 ) { // hibaellenőrzés: ha a fogadás nem sikerült
        fprintf(stderr, "Fogadó hiba\n");
        exit(11);
        }
    printf(" Szerver (%s:%d) nyugtázása:\n  %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port), buffer); // a szerver megerősítése

    if(buffer == NumValues) // ha a fogadott adatok száma egyezik a küldött adatok számával
    {
        
        bytes = sendto(s, &tomb, sizeof(tomb), flag, (struct sockaddr *) &server, server_size); // adatok küldése a szervernek
        if ( bytes <= 0 ) { // hibaellenőrzés: ha a küldés nem sikerült
            fprintf(stderr, "Küldési hiba\n");
            exit(10);
        }
        printf (" %i bájt elküldve a szervernek.\n", bytes-1); // küldött bájtok számának kiírása

        

        bytes = recvfrom(s, &buffer, sizeof(int), flag, (struct sockaddr *) &server, &server_size); // adatok fogadása a szervertől
        if ( bytes < 0 ) { // hibaellenőrzés: ha a fogadás nem sikerült
            fprintf(stderr, "Fogadó hiba\n");
            exit(11);
        }
        printf(" Szerver (%s:%d) nyugtázása:\n  %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port), buffer); // a szerver megerősítése

        if(buffer != sizeof(int) * NumValues) // ha a fogadott adatok száma nem egyezik meg a várt számmal
        {
            printf("Nem egyezik a visszakuldott meret bajtban");
            exit(12);
        }
    }
    else // ha a fogadott adatok száma nem egyezik meg a várt számmal
    {
        printf("Nem egyezik a visszakuldott merete a tombnek");
        exit(13);
    }

    close(s);
}

void stop(int sig){

    close(s);
    printf("\n A szerver leállt\n");
    exit(14);
    }



// 6. feldat 2. lépése
void ReceiveViaSocket()
{

    int bytes;                        
    int err;                          // hibakód
    int flag;                         // átviteli zászló
    char on;                          // sockopt opció
    int buffer;                       // adatpuffer
    int meret;
    int mehet = 0;
    unsigned int server_size;         // a sockaddr_in server hossza
    unsigned int client_size;         // a sockaddr_in client hossza
    struct sockaddr_in server;        // a szerver címe
    struct sockaddr_in client;        // a kliens címe

    

    on   = 1;
    flag = 0;
    server.sin_family      = AF_INET; // IP verzió
    server.sin_addr.s_addr = INADDR_ANY; // bármelyik hálózati interfész
    server.sin_port        = htons(3333); // a szerver portszáma
    server_size = sizeof server; // a szerver struktúra mérete
    client_size = sizeof client; // a kliens struktúra mérete
    signal(SIGINT, stop); // SIGINT jelre reagáló leállító függvény beállítása
    signal(SIGTERM, stop); // SIGTERM jelre reagáló leállító függvény beállítása

    


    s = socket(AF_INET, SOCK_DGRAM, 0 ); // UDP socket létrehozása
    if ( s < 0 ) { // hibaellenőrzés: ha a socket létrehozása nem sikerült
        fprintf(stderr, "Socket létrehozási hiba\n");
        exit(9);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on); // socket opciók beállítása
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);



    err = bind(s, (struct sockaddr *) &server, server_size); // socket címhez rendelése
    if ( err < 0 ) { // hibaellenőrzés: ha a hozzárendelés nem sikerült
        fprintf(stderr,"Hozzárendelési hiba\n");
        exit(15);
    }

    while(1){ // Végtelen ciklus a folyamatos szerver működéshez

        if(mehet == 0) // ha még nem lehet fogadni
        {
            printf("\n Várakozás az üzenetre\n"); // várakozás üzenet
            bytes = recvfrom(s, &meret, sizeof(int), flag, (struct sockaddr *) &client, &client_size ); // adatok fogadása
            if ( bytes < 0 ) { // hibaellenőrzés: ha a fogadás nem sikerült
                fprintf(stderr, "Fogadó hiba\n");
                exit(11);
            }
            printf ("meret megkapva, %d\n", meret); // fogadott méret kiírása
            mehet = 1; // lehetőség jelzése

            alarm(0); // időzítő kikapcsolása

            

            bytes = sendto(s, &meret, sizeof(int), flag, (struct sockaddr *) &client, client_size); // válasz küldése a kliensnek
            if ( bytes <= 0 ) { // hibaellenőrzés: ha a küldés nem sikerült
                fprintf(stderr, "Küldési hiba\n");
                exit(10);
            }
            printf("Visszaigazolás küldése a kliensnek\\n"); // visszaigazolás
        }
        else // ha lehet fogadni
        {
            int Values[meret]; // fogadott értékek tömbje
            printf("\n Várakozás az üzenetre\n");
            bytes = recvfrom(s, &Values, sizeof(int) * meret, flag, (struct sockaddr *) &client, &client_size ); // értékek fogadása
            if ( bytes < 0 ) { // hibaellenőrzés: ha a fogadás nem sikerült
                fprintf(stderr, "Fogadó hiba\n");
                exit(11);
            }
            printf ("Tomb megkapva, merete: %ld", sizeof(Values)); // fogadott tömb méretének kiírása
            mehet = 0; // lehetőség jelzése

            
            
            int visszakuldendo_meret = meret * sizeof(int); // visszaküldendő adatok mérete
            bytes = sendto(s, &visszakuldendo_meret, sizeof(int),flag, (struct sockaddr *) &client, client_size); // válasz küldése a kliensnek
            if ( bytes <= 0 ) { // hibaellenőrzés: ha a küldés nem sikerült
                fprintf(stderr, "Küldési hiba\n");
                exit(10);
            }
            printf("Visszaigazolás küldése a kliensnek\n"); // visszaigazolás kiírása

            int* Valuesp = malloc(sizeof(int) * meret); // fogadott értékek másolása dinamikus tömbbe
            for (int i = 0; i < meret; i++) // végigmegy a fogadott értékeken és másolja őket
            {
                Valuesp[i] = Values[i];
            }

            int meresek_kulonbsegei[meret]; // a mérések különbségeinek tömbje
            meresek_kulonbsegei[0] = 0;
            for (int i = 0; i < meret-2; i++) // végigmegy a fogadott értékeken és számolja a különbségeket
            {
                if(Valuesp[i+1] > Valuesp[i]) // ha az aktuális érték nagyobb mint az előző, akkor a különbség 1
                {
                    meresek_kulonbsegei[i+1] = 1;
                }
                else if(Valuesp[i+1] < Valuesp[i]) // ha kisebb, akkor -1
                {
                    meresek_kulonbsegei[i+1] = -1;
                }
                else if(Valuesp[i+1] == Valuesp[i]) // ha pedig megegyezik, akkor 0
                {
                    meresek_kulonbsegei[i+1] = 0;
                }
                printf("%d\n",Valuesp[i]); // kiírja az aktuális értéket
            }

            BMPcreator(meresek_kulonbsegei, meret); // BMP fájl létrehozása a különbségek alapján
            mehet = 0; // lehetőség jelzése
            free(Valuesp); // dinamikusan foglalt memória felszabadítása
        }
    }
}

#endif
