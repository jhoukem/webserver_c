		#include <stdio.h>
		#include <stdlib.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <netinet/ip.h>
		#include <sys/types.h>
		#include <sys/un.h>
		#include <string.h>
		#include <signal.h>
		#include <unistd.h>
		#include "http_request.h"


		#define BUFF_SIZE 1000


		/*int check_first_line(char ligne[])
		{
		 int nbMots = 1;
		 unsigned int i;
		 char arg1[50];
		 char arg2[50];
		 char arg3[50];

		 sscanf(ligne,"%s %s %s",arg1,arg2,arg3);
		 printf("arg 1 %s , arg2 %s arg3 %s\n",arg1,arg2,arg3);
		   if(strcmp(arg1,"GET") != 0)
		       return -1;
		  if(strlen(arg3) != 8)
		       return -1;
		  if(arg2[0] != '/' || strlen(arg2) != 1)
		       return -2;
		  if(strncmp(arg3,"HTTP/1.",7) != 0)
		       return -1;

		  printf("arg3[7]: %d arg3[8]: %d",arg3[7],arg3[8]);
		  if(arg3[7] != '0' && arg3[7] != '1')
		       return -1;
		  
		   for(i = 0; i < strlen(ligne); ++i){
		      if(ligne[i] == ' ')
			  nbMots++;
		   }
		   if(nbMots == 3)
		      return 0;	
		 return -1;
		}*/

			char * fgets_or_exit (char* buffer, int size, FILE * stream)
			{	
				while(fgets(buffer,size,stream) != NULL){
					if(strcmp(buffer,"\n") == 0 || strcmp(buffer,"\r\n") == 0)
						break;
					printf("<Cherokee>%s",buffer);
				}
					exit(0);	
			}

			

			int parse_http_request(const char *request_line,http_request *request){

				int nbMots = 1;
			    unsigned int i;
				char arg1[50];
		 		char arg2[50];
		 		char arg3[50];

		 		sscanf(request_line,"%s %s %s",arg1,arg2,arg3);
		 		printf("arg 1 %s , arg2 %s arg3 %s\n",arg1,arg2,arg3);
		  		 if(strcmp(arg1,"GET") != 0)
		    		return 0;
		  		 if(strlen(arg3) != 8)
		       		return 0;
		 		 if(strncmp(arg3,"HTTP/1.",7) != 0)
		       		return 0;
		 		 printf("arg3[7]: %d arg3[8]: %d",arg3[7],arg3[8]);
		  			if(arg3[7] != '0' && arg3[7] != '1')
		      		 return 0;
		      	for(i = 0; i < strlen(request_line); ++i){
		      		if(request_line[i] == ' ')
			  		nbMots++;
		   		}
		  		 if(nbMots != 3)
		      		return 0;
				
		      	request->method = HTTP_GET;
		      	request->major_version = 1;
		  		request->minor_version = arg3[7];
				request->url = malloc(strlen(arg2)+1);
				strcpy(request->url,arg2);


				return 1;
			}

			void skip_headers(FILE *client){
				char buffer[BUFF_SIZE];
				while (fgets(buffer,BUFF_SIZE,client) != NULL)   
				{
					if(strcmp(buffer,"\n") == 0 || strcmp(buffer,"\r\n") == 0)
						break;
					//printf("<Cherokee>%s",buffer);
				}
			}


			void send_status(FILE *client, int code, const char *reason_phrase){

					fprintf(client, "HTTP/1.1 %d %s\r\n",code,reason_phrase);
					fprintf(client, "Content-Length: %d\r\n", (int)strlen(reason_phrase));
					fprintf(client, "\r\n");
					fprintf(client, "%s",reason_phrase);
					fflush(client);	
			}

			void send_response (FILE * client , int code ,const char * reason_phrase  ,const char * message_body){
				send_status(client,code,reason_phrase);
				fprintf(client, "%s", message_body);
				fflush(client);	
			}


			


		void traiter_client(int socket_client)
		{
			const char * message_bienvenue ="Bonjour ,Marhaba , Onaya \n Bienvenu sur le serveur web  de C\n Ici nous avons à implementer un long message\n au moins dix lignes !!!\n vous rendez vous compte\n c'est quasiment impossible\n enfin sauf si on abuse\n des backslash n\n encore un petit dernier pr la route\nsdfsfsfjsdkjfskdjfksjfksjdfjs\nsdjhfgskjgfshdgfsdfhsfh\njsdkhfkjsdhfjksdhfjsdhfjsdhfjsdhfjhsjehehfjsehf\nshfsoieuijfsdjfosdujfioseufoijf\n\r\n" ;
			

			char buffer[BUFF_SIZE];

			const char *mode = "w+";

			int request_ok = 1;
			FILE *fi = fdopen(socket_client,mode);
			fgets(buffer,BUFF_SIZE,fi);

			/* verification de la requete: GET ... HTTP/1.x */
			 http_request requete;
			if(parse_http_request(buffer,&requete) != 1){
				request_ok = 0;
			}
			/* Passer les entetes */
			skip_headers(fi);

			/*Requete valide*/
			if (!request_ok)
			{	
				send_response(fi,400,"Bad Request","Bad request\r\n");
			}
			else if (requete.method == HTTP_UNSUPPORTED){
				send_response(fi,405,"Method Not Allowed","Method Not Allowed\r\n");
			}
			else if(strcmp(requete.url,"/") == 0){/*Ressource valide*/
				send_response(fi,200,"OK",message_bienvenue);
				//free(requete.url);
			}	
			else{/*Ressource invalide*/
				send_response(fi,404,"Not Found","Not Found \r\n");
			}		
			
			exit(0);
		}

		


