#include "chart.h"

int main(int argc, char* argv[])
{
    int uzemmod = 0; //0 - kuldo uzemmod, 1 - fogado uzemmod
    int kommod = 0; //0 - file, 1 - socket

    //1. feladat egyik része
    if(strcmp(argv[0], "./chart") == 0)
    {
        if(argc > 1)
        {
            if(strcmp(argv[1], "--version") == 0)
            {
                #pragma omp parallel sections
                {
                    #pragma omp section
                    {
                        printf("Verzio szam: \t%.1f\n", VERSION);
                    }
                    #pragma omp section
                    {
                        printf("Elkeszulesenek datuma: \t%s\n", DATE);
                    }
                    #pragma omp section
                    {
                        printf("Fejleszto neve: \t%s\n", AUTHOR);
                    }

                }

                return 0;
            }
            else if(strcmp(argv[1], "--help") == 0)
            {
                help();

                return 0;
            }
            else if((strcmp(argv[1], "-send") == 0) || (strcmp(argv[1], "-receive") == 0))
            {
                if(strcmp(argv[1], "-send") == 0)
                {
                    uzemmod = 0;
                }
                else
                {
                    uzemmod = 1;
                }

                if(argc > 2)
                {
                    if(strcmp(argv[2], "-file") == 0)
                    {
                        kommod = 0;
                    }
                    else if(strcmp(argv[2], "-socket") == 0)
                    {
                        kommod = 1;
                    }
                    else
                    {
                        help();
                        exit(1);
                    }
                }
            }
            else if((strcmp(argv[1], "-file") == 0) || (strcmp(argv[1], "-socket") == 0))
            {
                if(strcmp(argv[1], "-file") == 0)
                {
                    kommod = 0;
                }
                else
                {
                    kommod = 1;
                }

                if(argc > 2)
                {
                    if(strcmp(argv[2], "-send") == 0)
                    {
                        uzemmod = 0;
                    }
                    else if(strcmp(argv[2], "-receive") == 0)
                    {
                        uzemmod = 1;
                    }
                    else
                    {
                        help();
                        exit(1);
                    }
                }
            }
            else
            {
                help();
                exit(1);
            }
        }
    }
    else
    {
        printf("!!!A futtathato allomany neve chart kell legyen!!!\n");

        return 2;
    }

    if(uzemmod == 0)
    {
        int* meresek;
        int meret = Measurement(&meresek); //2. feladat 2. lépés, itt hívjuk meg a Measurement függvényt a mainban, mert küldő üzemmódban van
        if (kommod == 0)
        {
            SendViaFile(meresek, meret);
            return 0;
        }
        else if(kommod == 1)
        {
            SendViaSocket(meresek, meret);
        }
        free(meresek);
    }
    else if(uzemmod == 1)
    {
        while(1)
        {
            if (kommod == 0)
            {
                signal(SIGUSR1,ReceiveViaFile);
                sleep(10);
            }
            else if(kommod == 1)
            {
                signal(SIGINT,SignalHandler);
                signal(SIGUSR1,SignalHandler);
                ReceiveViaSocket();
                sleep(10);
            }
        }
    }

    return 0;
}
