#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include "my.h"

void traitement_signal(int sig)
{
  int status;
  
  waitpid(-1, &status, WNOHANG);
  printf("Signal %d recu \n", sig);  
}

void message_erreur() {
  printf("HTTP/1.1 400 Bad Request\r\n");
  printf("Connection: close\r\n");
  printf("Content-Length: 17\r\n");
}

void message_404() {
  printf("HTTP/1.1 404 Bad Request\r\n");
  printf("Connection: close\r\n");
  printf("Content-Length: 17\r\n");
}

void message_ok() {
  printf("HTTP/1.1 200 OK\r\n");
  printf("Connection: close\r\n");
  printf("Content-Length: 17\r\n");
}


void send_status(FILE *client , int code , const char *reason_phrase){
  char status[256];
  // status = "HTTP/1.1 " + code + " " + reason_phrase;
  sprintf(status, "HTTP/1.1 %d %s\r\n", code, reason_phrase);
  fprintf(client, "%s", status);
}

void send_response(FILE *client , int code , const  char *reason_phrase , const  char *message_body) {
  send_status(client, code, reason_phrase);
  if(message_body != NULL) {
    char content_length[256];
    sprintf(content_length, "Content-Length: %zu\r\n", strlen(message_body));
    fprintf(client, "Content-Type: text/html\r\n");
    fprintf(client, "%s", content_length);
    fprintf(client, "\n");
    fprintf(client, "%s", message_body);
  }
  fprintf(client, "\n");
}


int parse_http_request(const char *request_line, http_request *request)
{
  //Parsing en fonction des espaces changement du "check_buf"
  
  char *chaine = strdup(request_line);
  char *beg = strtok(chaine," ");
  char *target = strtok(NULL, " ");
  char *vers = strtok(NULL, " ");

  if (chaine == NULL || beg == NULL || vers == NULL)
    return -1;
  if (strncmp(beg,"GET",3) != 0)
    request->method = HTTP_UNSUPPORTED;
  if (strcmp(target,"/") == 0)
      request->target = target;
  if((strncmp(vers,"HTTP/1.0", 8)==0)){
    request->major_version=1;
    request->minor_version=0;
  }
  else if (strncmp(vers,"HTTP/1.1", 8)==0){
    request->major_version=1;
    request->minor_version=1;
  }
  else
    return -1;
  return 1;
}

char * fgets_or_exit ( char *buff, int size , FILE *stream) {
  char *buffer;

  if ((buffer = fgets(buff, size, stream)) == NULL)
    exit(1);
  return buffer;
}


void function(char *buff, int size , FILE *file, int fd, char *c, int socket_client,  http_request requete)
{
  int check = 0;

  requete.target ="/";
  fgets_or_exit(buff,size,file);
  //        check = parse_http_request(buff,&requete);

  fprintf(file,"%s",buff);
  fflush(file);
  while (fgets_or_exit(buff, size, file))
    {
      if ((buff[0] == '\n' || (buff[0] == '\r' && buff[1] == '\n')) && check == 1)
	{
	  int ret = 0;
	  while ((ret = read(fd, &c, 1024)) > 0)
	    write(socket_client, &c , ret);
	  message_ok();
	  send_response(file, 200,"OK","200 : OK");
	}
      else if ((buff[0] == '\n' || (buff[0] == '\r' && buff[1] == '\n')) && check == 0) {
	message_erreur();
	send_response(file,400,"Bad Request","400 : Bad Request");
      }
      else if ((buff[0] == '\n' || (buff[0] == '\r' && buff[1] == '\n')) && check == -1) {
	message_404();
	send_response(file,404,"Not Found","404 : Not Found");
      }
      printf("<SERVAPR> %s", buff);	      
    }
}

void skip_headers (FILE *client)
{
  char buffer[1024];
  while(buffer[0] != '\r' && buffer[0] != '\n')
    fgets_or_exit(buffer, 1024, client);
}


void initialiser_signaux() {
  struct sigaction sa;

  sa.sa_handler = traitement_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
      perror("sigaction(SIGCHLD)");
    }
  if ( signal ( SIGPIPE , SIG_IGN ) == SIG_ERR )
    {
      perror ( " signal " );
    }
}

int main()
{
  initialiser_signaux();
  int s = creer_serveur(8080);
  int socket_client;
  FILE *file;
  http_request requete;

  while (1)
    {
      socket_client = accept(s, NULL, NULL); 
      if (socket_client == -1)
	{
	  perror("accept");
	  /*   traitement d'erreur*/
	  return -1;
	}
      int pid = fork();
      char buff[1024];
      if (pid == 0)
      	{
	  file = fdopen(socket_client, "w+");  
	  int fd = open("../bienvenue", O_RDONLY);
	  char c[1024];
	  skip_headers(file);
	  function(buff, 1024, file,fd,c, socket_client, requete);
	}
      else 
	close(socket_client);
    }
  return 0;
}
