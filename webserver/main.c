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


void check_buf(char *buf)
{
  int i = 0;
  int w = 0;
  while (buf[i]) {
    if (buf[i] == ' ')
      w++;
    i++;
  }
  if (w == 2)
    {
      i= 0;
      char prems[3] = "";
      for ( ; i <= 2; i++)
	prems[i] = buf[i];
      i = 4;
      if (strncmp(prems, "GET", 3) == 0)
	{
	  for (; buf[i] != ' ' ;i++);
	  char ht[8];
	  int k = 0;
	  i++;
	  for (; k < 8; i++)
	    ht[k++] = buf[i];
	   printf("%s",ht);
	  if (strncmp(ht,"HTTP/1.",7) == 0)
	    {
	      i++;
	      if (buf[i] == '1' || buf[i] == '0')
		printf("BRAAAAAAAAAAAVO");
	    }
	}
    }
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
	  while (( ret= read(fd, &c, 1024)) > 0)
	    write(socket_client, &c , ret);
	  while ((fgets(buff, 1024, file)) != NULL)
	    {
	      check_buf(buff);
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
