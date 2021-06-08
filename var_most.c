#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>


typedef struct List
{
    int nr_samochod;
    struct List *next;
} list;
int d=0;
int nr_sam_most=0;//nr samochodu na moscie
int l_samochodow = 10;
int city_A=0,city_B=0,m_A=0,m_B=0;
list *kolejka_A = NULL;
list *kolejka_B = NULL;

list *miasto_A = NULL;
list *miasto_B = NULL;

list *lista_most = NULL;


pthread_cond_t przed_mostem;	//samochod jest przed mostem
pthread_cond_t za_mostem;		//samochod jest za mostem

pthread_mutex_t mutex_A; //mutex kolejki_A i miasta_A
pthread_mutex_t mutex_B; //mutex kolejki_B i miasta_B
pthread_mutex_t most_lock;		//stan mostu



void wypisz_lista(list *l )
{
    list *temp = l;
    while(temp!=NULL)
    {
        printf("%d ",temp->nr_samochod);
        temp = temp->next;
    }
    printf("\n");

}
void wypisz_kolejki()
{
    printf("Kolejka A:");
    wypisz_lista(kolejka_A);
    printf("Kolejka B:");
    wypisz_lista(kolejka_B);

    printf("Miasto A:");
    wypisz_lista(miasto_A);
    printf("Miasto B:");
    wypisz_lista(miasto_B);

}
void komunikat()
{    if(nr_sam_most==0)
    {
        printf("A-%d %d>>> [        ] <<<%d %d-B\n",
city_A,m_A,m_B,city_B);
    }
    else if (nr_sam_most>0)
    {
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",
city_A,m_A,nr_sam_most,m_B,city_B);
    }
    else
    {
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",
city_A,m_A,-nr_sam_most,m_B,city_B);
    }
    if(d==1)
    {
        wypisz_kolejki();

    }
}

void umiesc_w_liscie(list **l,int x )
{
    list * nowy= malloc(sizeof(list));
    nowy->nr_samochod = x;
    nowy->next = NULL;
    if(*l==NULL)
    {
        *l = nowy;
    }
    else
    {
        list * temp= *l;
        while(temp->next!=NULL)
        {
            temp=temp->next;
        }

        temp->next=nowy;
        return;
    }

}

void usun_z_listy(list **l,int x)
{
    if(*l==NULL)
    {
        return;
    }
    if((*l)->nr_samochod==x)
    {
        list * temp = (*l);
        list * temp2 = (*l)->next;
        free(temp);
        *l=temp2;
        return;


    }

    list * przed = *l;
    list * temp = *l;
    list * po = *l;
    while (temp->next != NULL)
    {
        if(temp->nr_samochod==x)
            break;
        przed=temp;
        temp = temp->next;
    }
    po=temp->next;
    free(temp);
    przed->next=po;
}

void usun_lista(list **l)
{
    list * temp = *l;
    list * del = *l;
    while (temp != NULL)
    {
        del=temp;
        temp = temp->next;
        free(del);
    }

    *l=NULL;
}
void czas_most()// czekaj od 0,5s do 1,5s
{

    int a = (rand()%11) * 100000 + 500000;

    usleep(a);
}

int podaj_pierwszy(list *l)
{if(l==NULL)
{return 0;

}
else return l->nr_samochod;

}

void miasto()// czekaj od 2s do 5s
{
    int a = (rand()%4) * 1000000 + 2000000;
    usleep(a);
}

void *most()
{ int numer=0;
    while(1)
{pthread_mutex_lock(&most_lock);
            //oczekiwanie na samochod przed mostem
 pthread_cond_wait(&przed_mostem, &most_lock);
numer=podaj_pierwszy(lista_most);

if(numer>0)
{pthread_mutex_lock(&mutex_A);
pthread_mutex_lock(&mutex_B);
usun_z_listy(&lista_most,numer);
            m_A--;
            nr_sam_most=numer;

            if(d==1)
            {
                usun_z_listy(&kolejka_A,numer);

            }
            komunikat();

            czas_most();
 pthread_mutex_unlock(&mutex_A);
 pthread_mutex_unlock(&mutex_B);
 pthread_cond_signal(&za_mostem);

}
else if(numer<0)
{pthread_mutex_lock(&mutex_A);
pthread_mutex_lock(&mutex_B);
usun_z_listy(&lista_most,numer);
            m_B--;
            nr_sam_most=numer;

            if(d==1)
            {
                usun_z_listy(&kolejka_B,numer);

            }
            komunikat();

            czas_most();
pthread_mutex_unlock(&mutex_A);
 pthread_mutex_unlock(&mutex_B);
 pthread_cond_signal(&za_mostem);

}

  pthread_mutex_unlock(&most_lock);
}
}
void *samochod(void * id)
{
    char c;
    int numer=(int) id;
    if(numer>0)
    {
        c='A';
    }
    else
    {
        c='B';
        numer=(-numer);
    }

    while(1)
    {
        miasto();

        if(c=='A')
        {//przejscie z miasta A do kolejki A
            pthread_mutex_lock(&mutex_A);
            pthread_mutex_lock(&mutex_B);

            city_A--;
            m_A++;


            if(d==1)
            {umiesc_w_liscie(&kolejka_A,numer);
            usun_z_listy(&miasto_A,numer);
            }
            komunikat();
            umiesc_w_liscie(&lista_most,numer);
            pthread_mutex_unlock(&mutex_A);
            pthread_mutex_unlock(&mutex_B);
            //wyslanie sygnalu do mostu

            pthread_cond_signal(&przed_mostem);

            pthread_mutex_lock(&mutex_B);
            //oczekiwanie na zejscie z mostu
            pthread_cond_wait(&za_mostem, &mutex_B);
            //samochod jest w miescie B
           city_B++;
            if(d==1)
            {
                umiesc_w_liscie(&miasto_B,numer);

            }
            komunikat();
            c='B';
            pthread_mutex_unlock(&mutex_B);

        }

        else if(c=='B')
        {//przejscie z miasta B do kolejki B
             pthread_mutex_lock(&mutex_A);
            pthread_mutex_lock(&mutex_B);

            city_B--;
            m_B++;


            if(d==1)
            {umiesc_w_liscie(&kolejka_B,numer);
            usun_z_listy(&miasto_B,numer);
            }
            komunikat();
            umiesc_w_liscie(&lista_most,-numer);
            pthread_mutex_unlock(&mutex_A);
            pthread_mutex_unlock(&mutex_B);
            //wyslanie sygnalu do mostu

            pthread_cond_signal(&przed_mostem);

            pthread_mutex_lock(&mutex_A);
            //oczekiwanie na zejscie z mostu
            pthread_cond_wait(&za_mostem, &mutex_A);
            //samochod jest w miescie B
           city_A++;
            if(d==1)
            {
                umiesc_w_liscie(&miasto_A,numer);

            }
            komunikat();
            c='A';
            pthread_mutex_unlock(&mutex_A);

        }
    }
}

int main(int argc,char*argv[])
{
    srand(time(NULL));
    int x = 0;
    while((x = getopt (argc, argv, "dN:"))!=-1)
    {
        switch(x)
        {
        //liczba samochodow
        case 'N':
            l_samochodow=atoi(optarg);
            break;
        //debug
        case 'd':

           d=1;
            break;

        }
    }


    pthread_mutex_init(&most_lock, NULL);
    pthread_mutex_init(&mutex_A, NULL);
    pthread_mutex_init(&mutex_B, NULL);
    pthread_cond_init(&przed_mostem, NULL);
    pthread_cond_init(&za_mostem, NULL);

    pthread_t *samochod_watki = malloc(sizeof(pthread_t)*l_samochodow);
    pthread_t most_watek;
    pthread_create(&most_watek, NULL, most, NULL);
    int i=0;
city_A=l_samochodow/2+(l_samochodow%2);
city_B=l_samochodow/2;

     if(d==1){
    for (i=0; i<l_samochodow; ++i)
    {
        if((i%2)==0)
        {umiesc_w_liscie(&miasto_A,i+1);}
        else
        {umiesc_w_liscie(&miasto_B,i+1);}
    }
     }
komunikat();
    for (i=0; i<l_samochodow; ++i)
    {
        if((i%2)==0)
        {pthread_create(&samochod_watki[i], NULL, samochod, (void *)(i+1));
        }
        else
        {pthread_create(&samochod_watki[i], NULL, samochod, (void *)(-i-1));
        }
    }

    for (i=0; i<l_samochodow; i++)
    {
        pthread_join(samochod_watki[i],NULL);
    }

    pthread_join(most_watek,NULL);
    pthread_mutex_destroy(&most_lock);
    pthread_mutex_destroy(&mutex_A);
    pthread_mutex_destroy(&mutex_B);
    pthread_cond_destroy(&przed_mostem);
    pthread_cond_destroy(&za_mostem);



     usun_lista(&kolejka_A);
    usun_lista(&kolejka_B);
      usun_lista(&miasto_A);
    usun_lista(&miasto_B);
usun_lista(&lista_most);
    return 0;
}
