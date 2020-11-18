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
unsigned char MACF1[6];
unsigned char MACF2[6];
unsigned char ethertype[2] = {0x0c,0x0c};
unsigned char IPOrigen[4];
unsigned char MaskSR[4];
unsigned char TramaEnv[1514];
unsigned char TramaRec[1514];

int obtenerDatos(int ds){
    struct ifreq nic;
    unsigned char name[50];
    int index;
    printf("\n Inserta el nombre de la interfaz: ");
    gets(name);
    strcpy(nic.ifr_name,name);
//Obtencion del Index
    if(ioctl(ds,SIOCGIFINDEX,&nic) == -1){
        perror("\n Error al obtener el Index");
        exit(0);
    } else {
        index = nic.ifr_ifindex;
        printf("\n El indice es: %d\n\n",index);
    }
//Obtencion de la MAC
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
//Obtencion de la IP
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
//Obtencion de la Mascara de Sub Red
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
    return index;
}

void estructuraTrama(unsigned char *trama){
    unsigned char nombre[]="Hernandez Sanchez Juan Rene";
    memcpy(trama+0,MACbroadcast,6);
    memcpy(trama+6,MACorigen,6);
    memcpy(trama+12,ethertype,2);
    memcpy(trama+14,nombre,27);
}

void enviarTrama(int ds, int index,unsigned char *trama){
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz,0x00,sizeof(interfaz));
    interfaz.sll_family=AF_PACKET; //Always AF_PACKET
    interfaz.sll_protocol=htons(ETH_P_ALL); //Phisical Layer Protocol
    interfaz.sll_ifindex=index; //Interface Number
    tam=sendto(ds,trama,60,0,(struct sockaddr*)&interfaz,sizeof(interfaz));
    if(tam==-1) {
        perror("\nError al enviar la trama");
        exit (0);
    } else {
        perror("\nExito al enviar la trama");
    }
}

void imprimirTrama(unsigned char *trama,int tam) {
    int i;
    printf("\n");
    for(i = 0; i < tam;i++) {
        if(i%16 == 0)
            printf("\n");
        printf("%.2x ",trama[i]);
    }
    printf("\n\n");
}

void filtrarTrama() {
    unsigned char select[1];
    int MF1[6];
    int MF2[6];
    printf("\nFiltrar MAC personal en Dir. Destino Precione (y/Y): ");
    gets(select);
    if(select[0] == 'y' || select[0] == 'Y') {
        memcpy(MACF1,MACorigen,6);
    } else {
        printf("\nIngrese Mac a Filtrar: ");
        scanf("%x:%x:%x:%x:%x:%x",&MF1[0],&MF1[1],&MF1[2],&MF1[3],&MF1[4],&MF1[5]);
        for(int i = 0; i < 6; i++){
            sprintf(&MACF1[i],"%x",MF1[i]);
        }
    }
    printf("\nFiltrar MAC personal en Dir. Fuente Precione (y/Y): ");
    gets(select);
    if(select[0] == 'y' || select[0] == 'Y') {
        memcpy(MACF2,MACorigen,6);
    } else {
        printf("\nIngrese Mac a Filtrar: ");
        scanf("%x:%x:%x:%x:%x:%x",&MF2[0],&MF2[1],&MF2[2],&MF2[3],&MF2[4],&MF2[5]);
        for(int i = 0; i < 6; i++){
            sprintf(&MACF2[i],"%x",MF2[i]);
        }
    }
    printf("La MAC Destino a filtrar es: ");
    for(int i = 0; i < 5; i++){
        printf("%.2x:",MACF1[i]);
    }
    printf("%.2x\n\n",MACF1[5]);
    printf("La MAC Origen a filtrar es: ");
    for(int i = 0; i < 5; i++){
        printf("%.2x:",MACF2[i]);
    }
    printf("%.2x\n\n",MACF2[5]);
}

void recibirTrama(int ds, unsigned char *trama) {
    int tam;
// Descriptor de Socket,buffer,tam buff, flag, struct de socket, tam socket
    filtrarTrama();
    while(1) {
        tam = recvfrom(ds,trama,1514,0,NULL,0);
        if(tam==-1) {
            perror("\nError al Recibir Trama");
            exit(0);
        } else {
            if(!memcmp(trama+0,MACF1,6) || !memcmp(trama+6,MACF2,6)) {
                imprimirTrama(trama,tam);
                //break;
            }
        }
    }
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
//    estructuraTrama(TramaEnv);
//    enviarTrama(packet_socket,index, TramaEnv);
    recibirTrama(packet_socket, TramaRec);
    close(packet_socket);
    return 0;
}

