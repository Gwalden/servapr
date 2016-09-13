#include <stdio.h>
#include <string.h>
#include "socket.h"

int creer_serveur(int port){
  int socket_serveur;

  socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_serveur == -1)
    {
      /* traitement de l'erreur */
      perror("socket_serveur");
    }

  struct sockaddr_in
  {
    sa_family_t    sin_family; /* address family: AF_INET */
    in_port_t      sin_port;   /* port in network byte order */
    struct in_addr sin_addr;   /* internet address */
  };

  struct in_addr
  {
    uint32_t  s_addr;  /* address in network byte order */
  };

  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET; /* Socket IPv4 */
  saddr.sin_port = htons(8080); /* port d'écoute */
  saddr.sin_addr.s_addr = INADDR_ANY; /* écoute sur toutes les interfaces */

  if(bind(socket_serveur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
      perror("bind socker_serveur");
      /* traitement de l'erreur */
    }

  if(listen(socket_serveur, 10) == -1)
    {
      perror("listen socket_serveur");
      /* traitement d'erreur */
    }
}
