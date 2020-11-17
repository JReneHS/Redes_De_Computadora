/*
 * =====================================================================================
 *
 *       Filename:  Socket_Raw_Servidor.c
 *
 *    Description:  Implementacion de un Socket raw a nivel OSI 2
 *
 *        Version:  1.0
 *        Created:  04/11/20 13:11:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Juan René Hernández Sánchez (), jhernandezs1509@alumno.ipn.mx
 *   Organization:  ESCOM
 *
 * =====================================================================================
 */
//Sockets RAW
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
//MISCELANEA
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//NETDEVICE
#include <sys/ioctl.h>
#include <net/if.h>

void obtenerDatos(int ds){
    struct ifreq nic;
    unsigned char name[50];
    printf("\n Inserta el nombre de la interfaz: ");
    gets(name);
    strcpy(nic.ifr_name,name);
    if(ioctl(ds,SIOCGIFINDEX,&nic) == -1){
        perror("\n Error al obtener el Index");
        exit(0);
    } else {
        printf("\n El indice es: %d\n\n",nic.ifr_ifindex);
    }
}

int main ( int argc, char *argv[] )
{
    int packet_socket;
//Abrir socket raw
    packet_socket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(packet_socket == -1){
        perror("\n Error al abrir el Socket");
        return 0;
    } else {
        perror("\n Exito al abrir el Socket");
    }
    obtenerDatos(packet_socket);
    close(packet_socket);
    return 0;
}

