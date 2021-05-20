#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
typedef struct List
{
    int nr_samochod;
    struct List *next=NULL;
} list;
typedef struct Arg
{
    int id;
    char miasto;
} arg;
int d=0;
int most=0;//nr samochodu na moscie
int liczbaSamochodow =25;
int city_A=0,city_B=0,m_A=0,m_B=0;
pthread_mutex_t most;
pthread_mutex_t strona_A_most;
pthread_mutex_t strona_B_most;


list *kolejka_A = NULL;
list *kolejka_B = NULL;

list *miasto_A = NULL;
list *miasto_B = NULL;

void komunikat()
{
    if(most==0)
    {
        printf("A-%d %d>>> [        ] <<<%d %d-B",city_A,m_A,most,city_B,m_B);
    }
    else if (most>0)
    {
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B",city_A,m_A,most,city_B,m_B);
    }
    else
    {
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B",city_A,m_A,-most,city_B,m_B);
    }
}

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
void umiesc_w_liscie(list *l,int x )
{
    list * temp = l;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = (list *) malloc(sizeof(list));
    temp->next->nr_samochod = x;
    temp->next->next = NULL;

}
void usun_z_listy(list *l,int x)
{
    list * temp = l;
    list * n = l;
    while (temp->next != NULL)
    {
        if(temp->nr_samochod==x)
            break;
        temp = temp->next;
    }
    n=temp->next;
    free(temp);
    l=n;
}

void usun_lista(list *l)
{
    list * temp = l;
    list * del = l;
    while (temp != NULL)
    {
        del=temp;
        temp = temp->next;
        free(del);
    }
    free(temp);
    l=null;
}
void most()// czekaj od 1s do 4s
{
    int a = (rand()%3) * 1000000 + 1000000;
    usleep(a);
}

void miasto()// czekaj od 0,5s do 3,5s
{
    int a = (rand()%30) * 100000 + 500000;
    usleep(a);
}


void *samochod(void *arguments)
{
    arg *arg = arguments;
    int id=arg->id;
    char c=arg->miasto;
    while(1)
    {
        miasto();
        if(c=="A")
        {
            pthread_mutex_lock(&strona_A_most);
            city_A--;
            m_A++;

            komunikat();
            if(d==1)
            {
                umiesc_w_liscie(kolejka_A,id);
            }
            pthread_mutex_unlock(&strona_A_most);

            pthread_mutex_lock(&most);
            pthread_mutex_lock(&strona_A_most);
            m_A--;
            most=id;
            if(d==1)
            {
                usun_z_listy(kolejka_A,id);
            }
            komunikat();

            pthread_mutex_unlock(&strona_A_most);

            most();
            pthread_mutex_lock(&strona_B_most);
            city_B++;
            most=0;
            if(d==1)
            {
                umiesc_w_liscie(miasto_B,id);
            }
            pthread_mutex_unlock(&most);
            pthread_mutex_unlock(&strona_B_most);

        }
        else if(c=="B")
        {
            pthread_mutex_lock(&strona_B_most);
            printf("A-%d %d>>> [>> %d >>] <<<%d %d-B",city_A,m_A,most,city_B,m_B);
            city_B--;
            m_B++;

            komunikat();
            if(d==1)
            {
                umiesc_w_liscie(kolejka_B,id);
            }
            pthread_mutex_unlock(&strona_B_most);

            pthread_mutex_lock(&most);
            pthread_mutex_lock(&strona_B_most);
            m_B--;
            most=-id;
            if(d==1)
            {
                usun_z_listy(kolejka_B,id);
            }
            komunikat();

            pthread_mutex_unlock(&strona_B_most);

            most();
            pthread_mutex_lock(&strona_A_most);
            city_A++;
            most=0;
            if(d==1)
            {
                umiesc_w_liscie(miasto_A,id);
            }
            pthread_mutex_unlock(&strona_A_most);
            pthread_mutex_unlock(&most);

        }
    }
}

void *watek_most(void *arguments)
{
    arg *arg = arguments;
    int id=arg->id;
    char c=arg->miasto;

    sem_wait(&most);
    pthread_mutex_lock(&most);


}

int main(int argc, char *argv[])
{
    srand(time(NULL));



    int x = 0;

    while(x = getopt (argc, argv, "N:d")!=-1)
    {
        switch(wybor)
        {
        //liczba samochodow
        case 'N':
            liczbaSamochodow=atoi(optarg);
            break;
        case 'd':
            d=1;
            break;

        }
    }

    pthread_t *samochod = malloc(sizeof(pthread_t)*liczbaSamochodow);

    pthread_mutex_init(&most;, NULL);
    pthread_mutex_init(&strona_A_most, NULL);
    pthread_mutex_init(&strona_B_most, NULL);
    sem_init(&sem_most,0,0);
    arg dane;
    for (i=0; i<liczbaSamochodow; ++i)
    {
        dane=(arg *) malloc(sizeof(arg));
        dane.id=i+1;
        if((i%2)==0)
        {
            dane.miasto="A";
            umiesc_w_liscie(miasto_A,i+1);
        }
        else
        {
            dane.miasto="B";
            umiesc_w_liscie(miasto_B,i+1);
        }

        pthread_create(&samochod[i], NULL, samochod(), (void *)&dane);

    }
    pthread_join(watek_most(),NULL);

    for (i=0; i<liczbaSamochodow; i++)
    {
        pthread_join(samochod[i],NULL);
    }
    pthread_mutex_destroy(&strona_A_most);
    pthread_mutex_destroy(&strona_B_most);
    pthread_mutex_destroy(&most);
    sem_destroy(&sem_most);
    usun_lista(kolejka_A);
    usun_lista(kolejka_B);
    usun_lista(miasto_A);
    usun_lista(miasto_B);
    return 0;
}
