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

void traitement_signal(int sig)
{
  int status;
  
  waitpid(-1, &status, WNOHANG);
  printf("Signal %d recu \n", sig);  
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


int check_buf(char *buf)
{
  int i = 0;
  int w = 0;
  while (buf[i]) {
    if (buf[i] == ' ') {
      w++;
    }
    i++;
  }
  if (w == 2)
    {
      i= 0;
      char prems[3] = "";
      for ( ; i <= 2; i++) {
	prems[i] = buf[i];
      }
      i = 4;
      if (strncmp(prems, "GET", 3) == 0)
	{
	  for (; buf[i] != ' ' ;i++);
	  char ht[8];
	  int k = 0;
	  i++;
	  for (; k < 8; i++) {
	    ht[k] = buf[i];
	    k++;
	  }
	  ht[k] = '\0';
	  if (strncmp(ht,"HTTP/1.",7) == 0)
	    {
	      if (ht[7] == '1' || ht[7] == '0') {
		return 1;
	      } else {
		return 0;
	      }
	    } else {
	    return 0;
	  }
	} else {
	return 0;
      }
    } else {
    return 0;
  }
}

void message_erreur() {
  printf("HTTP/1.1 400 Bad Request\r\n");
  printf("Connection: close\r\n");
  printf("Content-Length: 17\r\n");
}


void message_ok() {
  printf("HTTP/1.1 200 OK\r\n");
  printf("Connection: close\r\n");
  printf("Content-Length: 17\r\n");
}

int main()
{
  initialiser_signaux();
  int s = creer_serveur(8080);
  int socket_client;
  FILE *file;
  
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
	  int ret;
	  char c[1024];
	  fgets(buff, 1024, file);
	  int check = check_buf(buff);
	  while ((fgets(buff, 1024, file)) != NULL)
	    {
	      if ((buff[0] == '\n' || (buff[0] == '\r' && buff[1] == '\n')) && check == 1)
		{
		  while (( ret= read(fd, &c, 1024)) > 0)
		    write(socket_client, &c , ret);
		  message_ok();
		} 
	      else if ((buff[0] == '\n' || (buff[0] == '\r' && buff[1] == '\n')) && check == 0) {
		message_erreur();
	      }
	      check = check_buf(buff);
	      printf("<SERVAPR> %s", buff);	      
	    }

	  exit(0);
	}
      else 
	{      
	  close(socket_client);
	}
    }
  return 0;
}
