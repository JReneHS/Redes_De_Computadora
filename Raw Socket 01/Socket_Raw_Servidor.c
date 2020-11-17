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

unsigned char MACorigen[6];
unsigned char MACbroadcast[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char ethertype[2] = {0x0c,0x0c};
unsigned char IPOrigen[4];
unsigned char MaskSR[4];
unsigned char TramaEnv[1514];

int obtenerDatos(int ds){
    struct ifreq nic;
    unsigned char name[50];
    int index;
    printf("\n Inserta el nombre de la interfaz: ");
    gets(name);
    strcpy(nic.ifr_name,name);
    if(ioctl(ds,SIOCGIFINDEX,&nic) == -1){
        perror("\n Error al obtener el Index");
        exit(0);
    } else {
        index = nic.ifr_ifindex;
        printf("\n El indice es: %d\n\n",index);
    }
    if(ioctl(ds,SIOCGIFHWADDR,&nic) == -1){
        perror("Error al Obtener MAC");
        exit(0);
    } else {
        memcpy(MACorigen,nic.ifr_hwaddr.sa_data,6);
        printf("La MAC es: ");
        for(int i = 0; i < 5; i++){
            printf("%2x:",MACorigen[i]);
        }
        printf("%2x\n\n",MACorigen[5]);
    }
    if(ioctl(ds,SIOCGIFADDR,&nic) == -1){
        perror("Error al Obtener IP Origen");
        exit(0);
    } else {
        memcpy(IPOrigen,nic.ifr_addr.sa_data+2,4);
        printf("La IP Origen es: ");
        for(int i = 0; i < 3; i++){
            printf("%d:",IPOrigen[i]);
        }
        printf("%d\n\n",IPOrigen[4]);
    }
    if(ioctl(ds,SIOCGIFNETMASK,&nic) == -1){
        perror("Error al Obtener Mascara de sub Red");
        exit(0);
    } else {
        memcpy(MaskSR,nic.ifr_netmask.sa_data+2,4);
        printf("La Mascara de Sub Red Origen es: ");
        for(int i = 0; i < 3; i++){
            printf("%d:",MaskSR[i]);
        }
        printf("%d\n\n",MaskSR[4]);
    }
//Tarea: Obtener Dir IP y Mask Subred (Familia AF_INET)
    return index;
}

void estructuraTrama(unsigned char *trama){
    memcpy(trama+0,MACbroadcast,6);
    memcpy(trama+6,MACorigen,6);
    memcpy(trama+12,ethertype,2);
    memcpy(trama+14,"Hernandez Sanchez Juan Rene",27);
}

void enviarTrama(int ds, int index,unsigned char *trama){

}

int main ( int argc, char *argv[] )
{
    int packet_socket;
    int index;
//Abrir socket raw
    packet_socket = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(packet_socket == -1){
        perror("\n Error al abrir el Socket");
        return 0;
    } else {
        perror("\n Exito al abrir el Socket");
    }
    index = obtenerDatos(packet_socket);
    estructuraTrama(TramaEnv);
    enviarTrama(packet_socket,index, TramaEnv);
    close(packet_socket);
    return 0;
}

