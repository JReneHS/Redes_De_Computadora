/*
 * =====================================================================================
 *
 *       Filename:  protocoloARP.cpp
 *
 *    Description:  protocoloARP
 *
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Juan Rene Hernandez Sanchez, jhernandezs1509@alumno.ipn.mx
 *   Organization:  ESCOM
 *
 * =====================================================================================
 */
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>

//##################################################################################################
//              Variables a utilizar
//
//################################################################################################3#

//unsigned char ethertype[2]={0x0c, 0x0c};
unsigned char MACorigen[6];

//solicitud de ARP 
unsigned char MACdestino[6];  //MAC a conocer 
unsigned char ethertype[2]={0x08,0x06}; // puerto 0086
unsigned char codARPresp[2]={0x00,0x02};
//Trama a enviar FFFFFFFFFFFF ya que no conosco la MAC destino
unsigned char tramaEnv[60]={0xff,0xff,0xff, 0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,
			    0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
			    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,'G','S','Q'};
unsigned char tramaRec[60];

unsigned char IPorigen[4];//={10.0.0.2};
unsigned char IPdestino[4];//={127.0.0.1};
//celular y pc

/*#####################################################################################################

Funciones a utilizar

######################################################################################################################*/


void imprimirMiTrama(unsigned char *paq, int len,FILE *guarda)
{
    int i;
    for(i=0; i<len; i++)
    {
        if(i%16==0)
        printf("");
        fprintf(guarda,"%.2x ", paq[i]);
    }
    fputs("\n",guarda);
}

void armarTrama(unsigned char *trama)
{

    //Generando encabezado MAC
    
    memcpy(trama+6, MACorigen, 6);
 
    //eL MMS ARP

    memcpy(trama+22, MACorigen, 6);
    memcpy(trama+28, IPorigen, 4);
    memset(trama+32, 0x00, 6);
    memcpy(trama+38,IPdestino, 4);
}

//enviar la trama que necesitamos a la ip que nosotros queramos
void enviar( int ds, int index, unsigned char *trama)
{
    
    //llenar nuestra estrucutra sockaddr
    int tam;
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));
    interfaz.sll_family=AF_PACKET;
    interfaz.sll_protocol=htons(ETH_P_ALL);
    interfaz.sll_ifindex=index;
    
    //se hace el envio
    tam=sendto(ds, trama, 60, 0, (struct sockaddr *)&interfaz, sizeof(interfaz));

    if(tam==-1)
    {
        perror("\nError al enviar");
        exit(0);
    }
    
    else
    {
        perror("\n Exito al enviar");
    }
}

//recibir las tramas que incluye ip y mac
void recibir(int ds, unsigned char *trama,FILE *guarda,char ip[20])
{
	int bandera=0;

    //variables de medicion de tiempo
    struct timeval start, end;
    long mtime=0, seconds, useconds;

    // recvfrom
    gettimeofday(&start,NULL);
    while(mtime<200)
    {
        int tam;  
        tam=recvfrom(ds, trama, 60, MSG_DONTWAIT, NULL, 0);
        
        //Si no llegana
        if(tam ==-1)
        {
            //perror("\nError al recibir");
        }

        //Llego algo
        else
        {
            //Filtros
            if(!memcmp(trama,MACorigen,6)&&!memcmp(trama+12,ethertype,2)&&!memcmp(trama+20,codARPresp,2)&&!memcmp(trama+28,IPdestino,4))
            {
                //bandera=1;
                //&&!memcmp(trama+12,ethertype,2)&&!memcmp(trama+20,codARPresp,2)&&!memcmp(trama+28,IPdestino,4)
                fprintf(guarda,"La IP es: %s",ip); //imprimimos directamente en el archivo de texto
                fputs("************SE RECIBIO LA TRAMA************\n",guarda);
                memcpy(MACdestino,trama+22,6);
                fputs("La MAC es:",guarda);
                imprimirMiTrama(MACdestino,6,guarda);
                fputs("******************************************\n\n",guarda);
                bandera=1;
            }
        }
        gettimeofday(&end,NULL);
        seconds = end.tv_sec - start.tv_sec;
        useconds = end.tv_usec -start.tv_usec;
        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        if(bandera==1)
        {
            break;
        }
    }
}

int obtenerDatos(int ds, char ip[])
{
    struct ifreq nic;
    unsigned char nombre[20]={"wlp1s0"};//poner personal
    int i, index;

    inet_aton(ip,IPdestino);
	//memcpy(IPdestino,ipdest,4);
    strcpy(nic.ifr_name, nombre);
    if(ioctl(ds, SIOCGIFINDEX, &nic)==-1)
    {
        perror("\nerror al obtener el index");
    }

    else
    { 
        index=nic.ifr_ifindex;
        if(ioctl(ds, SIOCGIFHWADDR, &nic)==-1)
        {
            perror("\nError al obtener la MAC");
            exit(0);
        }
        
        else
        {
            memcpy(MACorigen, nic.ifr_hwaddr.sa_data+0, 6);
            for(i=0; i<6; i++)
            {
                printf("%.2x:", MACorigen[i]);
            }
	    }
	 
        if(ioctl(ds, SIOCGIFADDR, &nic)==-1)
        {
            perror("\nError al obtener la IP");
            exit(0);
        }
        
        else
        {
    	    memcpy(IPorigen,nic.ifr_addr.sa_data+2,4);
        }
    }
    return index;
}



/*"#######################################################################################################
Cuerpo 
######################################################################################################################"*/
int main ()
{    
    char ip[20];
    int packet_socket, indice;
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //abrimos nuestro socket
    
    //abre el socket
    if(packet_socket==-1)
    {
        perror("Error al abrir el socket");
        exit(0);
    }

    else
    {
    
        //abriendo el archivo
        FILE *archivo =fopen("ips.txt","r");
        FILE *guarda =fopen("salida.txt","w");
        
        if(archivo ==NULL)
        {
            printf("Error al abrir el archivo");
            return 1;
        }

        else
        {
            //Opteniendo las IPs
            perror("\nExito al abrir el socket\n");
            while (fgets(ip,20,archivo) != NULL)//encontrar final de archivo
            {
                printf("La Ip es: %s \n",ip);
                indice=obtenerDatos(packet_socket,ip);//se le manda el descriptor de socket
                armarTrama(tramaEnv);
                enviar(packet_socket, indice, tramaEnv);
                recibir(packet_socket, tramaRec,guarda,ip);       
            }
            
        }
        fclose(guarda);
        fclose(archivo);
    }
    
    close(packet_socket); //cerramos nuestro socket

    return 0;
}

