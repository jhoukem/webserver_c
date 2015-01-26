#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFF_SIZE 1000


void traitement_signal(int sig)
{
 
  while(waitpid(-1,NULL,WNOHANG) == 0);
  printf("Signal %d reçu\n" ,sig);
}


void initialiser_signaux(void)
{
  if(signal(SIGPIPE,SIG_IGN) == SIG_ERR)
   {
     perror("signal");
   }
 struct sigaction sa;
  sa.sa_handler = traitement_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa ,NULL) == -1)
  {
   perror("sigaction(SIGCHLD)");
  }

}


void traiter_client(int socket_client)
{
const char * message_bienvenue ="Bonjour ,Marhaba , Onaya \n Bienvenu sur le serveur web de C\n Ici nous avons à implementer un long message\n au moins dix lignes !!!\n vous rendez vous compte\n c'est quasiment impossible\n enfin sauf si on abuse\n des backslash n\n encore un petit dernier pr la route\nsdfsfsfjsdkjfskdjfksjfksjdfjs\nsdjhfgskjgfshdgfsdfhsfh\njsdkhfkjsdhfjksdhfjsdhfjsdhfjsdhfjhsjehehfjsehf\nshfsoieuijfsdjfosdujfioseufoijf\n" ;
 sleep(1);

 write(socket_client,message_bienvenue,strlen(message_bienvenue));
/*int taille;*/

char buffer[BUFF_SIZE];

const char *mode = "w+";
FILE *fi = fdopen(socket_client,mode);

while (  fgets(buffer,BUFF_SIZE,fi) != NULL)   
   {
    fprintf(fi,"<Cherokee>%s\n",buffer);
   }
/*
 while ((taille = read(socket_client, buffer, BUFF_SIZE))>0) //fgets
 {
	write(socket_client, buffer, taille); //fprint
 }*/
exit(0);
}


int creer_serveur (int port)
{
  
  int socket_serveur ;
  int optval = 1;

  socket_serveur = socket(AF_INET,SOCK_STREAM,0);
  //socket(SERVEUR, PF_INET, SOCK_STREAM, getprotobyname('http'));
  //bind(SERVEUR, $adresse)
  if (socket_serveur == -1)
    {
      perror ("socket_serveur");
      return -1;
    }
  struct sockaddr_in saddr;
  saddr.sin_family=AF_INET; /* Socket ipv4 */
  saddr.sin_port=htons(port); /* Port d ’ écoute */
  saddr.sin_addr.s_addr=INADDR_ANY; /* écoute sur toutes les interfaces */
 
if (setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
{
	perror ("Can not set SO_REUSEADDR option ");
	return -1;
}
if (bind(socket_serveur,(struct sockaddr *) &saddr , sizeof(saddr)) == -1)
    {
      perror ( "bind socker_serveur");
      return -1;
      /* traitement de l ’ erreur */
    }

  if(listen(socket_serveur,10) == -1)
    {
      perror("listen socket_serveur");
      return -1;
      /* traitement d ’ erreur */
    }
	
  return socket_serveur;
}




