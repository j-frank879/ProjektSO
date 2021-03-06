
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

int d=0;//debug
int nr_sam_most=0;//nr samochodu na moscie
int l_samochodow = 10;// liczba krazacych samochodow
int city_A=0,city_B=0,m_A=0,m_B=0; //liczba samochodu w miescie A,miescie B,kolejce A,kolejce B
list *kolejka_A = NULL; //ktore samochody w kolejce A
list *kolejka_B = NULL;//ktore samochody w kolejce B

list *miasto_A = NULL;//ktore samochody w miasto A
list *miasto_B = NULL;//ktore samochody w miasto B

pthread_mutex_t mutex_most; //mutex mostu(nr_sam_most)
pthread_mutex_t strona_A_most; //mutex kolejki A (m_A)
pthread_mutex_t strona_B_most; //mutex kolejki B (m_B)
pthread_mutex_t A_miasto; //mutex miasto A (city_A)
pthread_mutex_t B_miasto; //mutex miasto B (city_B)

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
void czas_most()// czekaj od 1s do 4s
{
    int a = (rand()%4) * 1000000 + 1000000;

    usleep(a);
}

void miasto()// czekaj od 0,5s do 3,5s
{
    int a = (rand()%31) * 100000 + 500000;
    usleep(a);
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
            //samochod przemieszcza sie z miasta A do kolejki A
        {   pthread_mutex_lock(&A_miasto);
            pthread_mutex_lock(&strona_A_most);

            city_A--;
            m_A++;


            if(d==1)
            {
                umiesc_w_liscie(&kolejka_A,numer);
            usun_z_listy(&miasto_A,numer);

            }
            komunikat();
            pthread_mutex_unlock(&strona_A_most);
            pthread_mutex_unlock(&A_miasto);
//samochod przemieszcza sie z kolejki A na most
            pthread_mutex_lock(&mutex_most);
            pthread_mutex_lock(&strona_A_most);

            m_A--;
            nr_sam_most=numer;

            if(d==1)
            {
                usun_z_listy(&kolejka_A,numer);

            }
            komunikat();

            pthread_mutex_unlock(&strona_A_most);

            czas_most();
            //samochod przemieszcza sie z  mostu do miasta B
            pthread_mutex_lock(&B_miasto);
            city_B++;
            nr_sam_most=0;

             c='B';
            if(d==1)
            {
                umiesc_w_liscie(&miasto_B,numer);

            }
            komunikat();
            pthread_mutex_unlock(&mutex_most);
            pthread_mutex_unlock(&B_miasto);

        }
        else if(c=='B')
        {//samochod przemieszcza sie z miasta B do kolejki B
            pthread_mutex_lock(&B_miasto);
            pthread_mutex_lock(&strona_B_most);

            city_B--;
            m_B++;


            if(d==1)
            {   usun_z_listy(&miasto_B,numer);
                umiesc_w_liscie(&kolejka_B,numer);

            }
            komunikat();
            pthread_mutex_unlock(&strona_B_most);
            pthread_mutex_unlock(&B_miasto);
//samochod przemieszcza sie z kolejki B na most
            pthread_mutex_lock(&mutex_most);
            pthread_mutex_lock(&strona_B_most);

            m_B--;
            nr_sam_most=-numer;

            if(d==1)
            {
                usun_z_listy(&kolejka_B,numer);

            }
komunikat();

            pthread_mutex_unlock(&strona_B_most);

            czas_most();
             //samochod przemieszcza sie z  mostu do miasta A
            pthread_mutex_lock(&A_miasto);
            city_A++;
            nr_sam_most=0;

            c='A';
            if(d==1)
            {
                umiesc_w_liscie(&miasto_A,numer);

            }
            komunikat();
            pthread_mutex_unlock(&mutex_most);
            pthread_mutex_unlock(&A_miasto);

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
    pthread_mutex_init(&mutex_most, NULL);
    pthread_mutex_init(&strona_A_most, NULL);
    pthread_mutex_init(&strona_B_most, NULL);
    pthread_mutex_init(&A_miasto, NULL);
    pthread_mutex_init(&B_miasto, NULL);

    pthread_t *samochod_watki = malloc(sizeof(pthread_t)*l_samochodow);
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

    pthread_mutex_destroy(&strona_A_most);
    pthread_mutex_destroy(&strona_B_most);
    pthread_mutex_destroy(&A_miasto);
    pthread_mutex_destroy(&B_miasto);
    pthread_mutex_destroy(&mutex_most);

     usun_lista(&kolejka_A);
    usun_lista(&kolejka_B);
      usun_lista(&miasto_A);
    usun_lista(&miasto_B);

    return 0;
}

