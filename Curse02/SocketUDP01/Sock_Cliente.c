#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
/***********************************************************************************
 *	Alumno:		Hernandez Sanchez Juan Rene
 *	Boleta:		2016630187
 *	Materia: 	Redes 1
 *	Escuela:	ESCOM
 *	Grupo:		2CM17
 *	Semestre:	2021-2
 ***********************************************************************************
*/


int main(int argc, const char *argv[])
{
  int udp_socket, lbind, tam;
  unsigned char msj[100]="Hola Paps 7w7r jaja xD";
  struct sockaddr_in local, remota;
  udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(udp_socket == -1)
  {
    perror("\nError al Abrir el Socket");
    exit(1);
  }
  else
  {
    perror("\nExito al Abrir el socket");
    local.sin_family=AF_INET; //la Familia
    local.sin_port=htons(0); //puerto efimero
    local.sin_addr.s_addr=INADDR_ANY;//Caso especifico para local
    //Si quieres trabajar con otras interfaces se tienen que declarar
    //una forma esta en "man inet_aton"
    lbind=bind(udp_socket, (struct sockaddr *)&local, sizeof(local));
    if(lbind == -1)
    {
      perror("\nError en el Bind");
      exit(1);
    }
    else
    {
      perror("\nExito en el Bind");
      remota.sin_family=AF_INET; //la Familia
      remota.sin_port=htons(53); //Puerto especifico
      remota.sin_addr.s_addr=inet_addr("8.8.8.8");
      tam=sendto(udp_socket, msj, 23, 0, (struct sockaddr *)&remota, sizeof(remota));
      if(tam == -1)
      {
        perror("\nError al enviar mensaje");
        exit(1);
      }
      else
      {
        perror("\nExito al enviar mensaje");
      }
    }
  }
  close(udp_socket);
  return 0;
}

