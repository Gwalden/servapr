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


void initialiser_signaux() {
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
      if (pid == 0)
      	{
	      int fd = open("../bienvenue", O_RDONLY);
	      char c[1024];
	      int ret;
	      while (( ret= read(fd, &c, 1024)) > 0)
		write(socket_client, &c , ret);
	      while ((ret = read(socket_client, &c,1024)) > 0)
		write(socket_client, &c, ret);
	    }
	  else 
	    {
	      close(socket_client);
	    }
    }
      return 0;
}
