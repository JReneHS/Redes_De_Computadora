#include<sys/socket.h>
#include<linux/if_packet.h>
#include<net/ethernet.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<net/if.h>
#include<unistd.h>
#include<string.h>
#include <stdbool.h>

void EnviarTrama( int ,int ,unsigned char *);
void ImprimeTrama(unsigned char * ,int);
void ObtenerMacOrigen(int);
int ObtenerIndice( int);
int ObtenerIndice( int);
void ObtenerIpOrigen(int);
void estructuraARPsol(unsigned char *);
int filtroARP(unsigned char *, int );
void recibeARPresp(int ds , unsigned char *);
void ObtenerMacDestino ( unsigned char *);

struct ifreq nic;
unsigned char tramaARPsol[60] = { 0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,'G','S','Q'};  

// El mensaje de solicitud de ARP empieza a partir del byte 14 y termina hasta antes del byte ‘G’.

unsigned char MACorigen[6];
unsigned char MACdestino[6];
unsigned char IPorigen[4];
unsigned char IPdestino[4];
unsigned char tramaARPresp[1514];
unsigned char etherARP[2]= {0x08,0x06};
unsigned char codARPPresp[2] = {0x00,0x02};

// En la función principal main() del programa se puede observar que después de //abrir el socket de paquetes y obtener los datos de la interfaz de red que se necesitan: índice, dirección MAC y dirección IP, se manda llamar a la función estructuraARPsol()que es donde se llenan los campos tanto del encabezado MAC y del mensaje de solicitud de ARP, y finalmente ya estructurada la trama de solicitud de ARP, se procede a enviarla.

int main (){
    int packet_socket,indice;
    char nombre[10];
    packet_socket=socket ( AF_PACKET, SOCK_RAW , htons ( ETH_P_ALL) );
    if ( packet_socket == -1 ){
        perror("\nError al abrir el socket");
        exit(1);
    }
    else{
        perror("\nExito al abrir el socket");
        printf("\nInserta el nombre de la interfaz: ");
        scanf("%s",nombre);
        printf("Escriba la direccion IP destino: ");
        scanf("%d.%d.%d.%d",IPdestino,IPdestino+1,IPdestino+2,IPdestino+3);
        strcpy(nic.ifr_name,nombre);
        indice=ObtenerIndice(packet_socket);
	ObtenerMacOrigen(packet_socket);
        ObtenerIpOrigen(packet_socket);
        
        //for para agregar bucle (has)
        
        estructuraARPsol(tramaARPsol);
        printf("\n**********La trama que se envia es*********\n");
        ImprimeTrama(tramaARPsol,60);
        EnviarTrama(packet_socket,indice,tramaARPsol);
        recibeARPresp(packet_socket,tramaARPresp);
        printf("\n**********La trama que se recibe es*********\n");
        
        //guardar datos en base de datos
        ImprimeTrama(tramaARPresp,60);
        ObtenerMacDestino(tramaARPresp);
    
        
    }
    close(packet_socket);
return 0;
}

void EnviarTrama( int ds,int index,unsigned char *paq){
    int tam;
    struct sockaddr_ll capaEnlace;
    memset(&capaEnlace,0x00,sizeof(capaEnlace));
    capaEnlace.sll_family=AF_PACKET;
    capaEnlace.sll_protocol=htons(ETH_P_ALL);
    capaEnlace.sll_ifindex=index;
    tam = sendto(ds,paq,60,0,(struct sockaddr*)&capaEnlace,sizeof(capaEnlace));
    if(tam==-1){
        perror("\nError al enviar trama");
        exit(1);
    }
    else
        perror("\nExito al enviar trama");
}

void ImprimeTrama(unsigned char *trama, int tam){
    int i;
    for(i=0;i<tam;i++){
        if(i%16==0)
            printf("\n");
	printf(" %.2x",trama[i]);
    }	
    printf("\n");
}

int ObtenerIndice( int ds){
    int indice;
    if ( ioctl(ds,SIOCGIFINDEX,&nic)==-1){
	perror("\nError al obtener el indice");
	exit(1);
    }
    else{
        indice=nic.ifr_ifindex;
        printf("\nEl indice es: %d \n",indice);
    }
return indice;
}

void ObtenerMacDestino ( unsigned char *trama){
    int i;
    memcpy(MACdestino,trama+6,6);
    printf("\nLa direccion MAC destino es: ");
    for(i=0;i<6;i++)
        printf("%.2x:",MACdestino[i]);			
    printf("\n");
}

void ObtenerMacOrigen(int ds){
    int i;
    if ( ioctl(ds,SIOCGIFHWADDR,&nic)==-1){
        perror("\nError al obtener la MAC");
        exit(1);
    }
    else{
        memcpy(MACorigen,nic.ifr_hwaddr.sa_data+0,6);
        printf("La direccion MAC origen es: ");
        for(i=0;i<6;i++)
            printf("%.2x:",MACorigen[i]);			
        printf("\n");
    }
}

void ObtenerIpOrigen(int ds){
    int i;
    if ( ioctl(ds,SIOCGIFADDR,&nic) ==-1){
        perror("\nError al obtener la IP");
        exit(1);
    }
    else{
        memcpy(IPorigen,nic.ifr_addr.sa_data+2,4);
        printf("La direcciÃ³n IP origen es: ");
        for (i=0;i<4;i++)
            printf("%d.",IPorigen[i]);
        printf("\n");
    }
}

// Para estructurar completamente la solicitud de ARP se llama a la función estructuraARPsol(), en donde se copian los campos faltantes del encabezado MAC y mensaje de ARP.

void estructuraARPsol(unsigned char * trama){
        
    //encabezado MAC
        memcpy(trama+6,MACorigen,6);
    //mensaje de ARP    
        memcpy(trama+22,MACorigen,6);
        memcpy(trama+28,IPorigen,4);
        memset(trama+32,0x00,6);
        memcpy(trama+38,IPdestino,4);
}

// La función filtroARP() regresa un valor cero si la trama no es una respuesta de ARP y un valor uno si la trama es una respuesta de ARP valida según lo mostrado en el siguiente código.

int filtroARP(unsigned char *paq, int len){
    if(!memcmp(paq,MACorigen,6))
    {
        if(!memcmp(paq+12,etherARP,2))
            if(!memcmp(paq+20,codARPPresp,2))
                if(!memcmp(paq+28,IPdestino,4))
             		return 1;
    }     
    else         
    	return 0;
    
}

// La función recibeARPresp() llama a la función recvfrom(), la cual estará recibiendo tramas que en la dirección MAC destino se encuentre la dirección MAC de unicast de la interfaz, o una dirección MAC de broadcast o de multicast. Pero como solo interesa recibir una respuesta de ARP hacia la terminal, entonces en principio nos interesa recibir tramas que en el campo dirección MAC destino se encuentre la dirección MAC de la interfaz de red de la terminal.

//cambiar funcion bloqueante y respetar filtros
void recibeARPresp(int ds , unsigned char *trama){
        int tam, bandera=0;
        while(1){
            tam=recvfrom(ds,trama,1514,0,NULL,0);
            if(tam==-1)
                perror("\nError al recibir");
            else{
                bandera=filtroARP(trama,tam);
                if (bandera==1)
                    break;
            }
        }
}
