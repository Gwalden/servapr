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

void envoi_client_serv(char *buf, FILE* file)
{
  while ((buf = fgets(buf, 1024, file)) != NULL)
    {
      fprintf(file,"<Servapr> %s", buf);
      fflush(file);
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
	  envoi_client_serv(buff,file);
	  exit(0);
	}
      else 
	{      
	  close(socket_client);
	}
    }
  return 0;
}
