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



//pthread_cond_t strona_A_most; 	//samochody oczekujace po stronie miasta A
//pthread_cond_t strona_B_most;		//--||-- B
pthread_cond_t A_miasto;		//samochody w miescie A
pthread_cond_t B_miasto;		//--||-- B

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
        pthread_mutex_lock(&most_lock);
        if(c=='A')
        {   
		
		city_A--;
		
            while(city_A<0)
            	pthread_cond_wait(&A_miasto, &most_lock);

            m_A++;
            
            
            if(d==1)
            {
                umiesc_w_liscie(&kolejka_A,numer);
            usun_z_listy(&miasto_A,numer);
            }
komunikat();
	
		
            
            m_A--;	
            /*while(m_A<0)
            	pthread_cond_wait(&A_miasto, &most_lock);*/
            

            
            
            nr_sam_most=numer;

            if(d==1)
            {
                usun_z_listy(&kolejka_A,numer);
            }
komunikat();


            czas_most();
            
            
            
            while(city_B<=0)
            	pthread_cond_wait(&B_miasto, &most_lock);

            
            city_B++;
            nr_sam_most=0;

             c='B';
            if(d==1)
            {
                umiesc_w_liscie(&miasto_B,numer);
            }
komunikat();

	pthread_mutex_unlock(&most_lock);
        
        }
        
        else if(c=='B')
        {
        	
        	city_B--;
		
            while(city_B>=0)
            	pthread_cond_wait(&B_miasto, &most_lock);
            	
            	
		m_B++;
            
            
            

            if(d==1)
            {
                umiesc_w_liscie(&kolejka_B,numer);
            usun_z_listy(&miasto_B,numer);
            }
komunikat();
	
		
            
            
            while(m_B<0)
            	pthread_cond_wait(&B_miasto, &most_lock);
            

            
            m_B--;
            nr_sam_most=numer;

            if(d==1)
            {
                usun_z_listy(&kolejka_B,numer);
            }
komunikat();


            czas_most();
            pthread_mutex_lock(&most_lock);
            
            
            while(city_A<=0)
            	pthread_cond_wait(&A_miasto, &most_lock);

            
            city_A++;
            nr_sam_most=0;

             c='A';
            if(d==1)
            {
                umiesc_w_liscie(&miasto_A,numer);
            }
komunikat();

		pthread_mutex_unlock(&most_lock);
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
    pthread_cond_init(&A_miasto, NULL);
    pthread_cond_init(&B_miasto, NULL);

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

    pthread_mutex_destroy(&most_lock);
    pthread_cond_destroy(&A_miasto);
    pthread_cond_destroy(&B_miasto);

     usun_lista(&kolejka_A);
    usun_lista(&kolejka_B);
      usun_lista(&miasto_A);
    usun_lista(&miasto_B);

    return 0;
}

