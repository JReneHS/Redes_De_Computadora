/*
 * =====================================================================================
 *
 *       Filename:  Socket_Server_NOC_UDP.c
 *
 *    Description: Socket para Servidor No Orientado a Conexion 
 *
 *        Version:  1.0
 *        Created:  21/10/20 14:02:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

struct superSocket {
    int udp_socket, lbind, tam, lrecv;
    unsigned char msj[512];
    unsigned char paqRec[512];
    struct sockaddr_in SERVIDOR, CLIENTE;
};

void *Recive(void *arg)
{
    struct superSocket *super = (struct superSocket*)arg;
    do
    {
        fflush(stdin);
        super->lrecv = sizeof(struct sockaddr_in);
        super->tam = recvfrom(super->udp_socket, super->paqRec, 512, 0, (struct sockaddr *)&super->CLIENTE, &super->lrecv); 
        if(super->tam == -1)
        {
            perror("\nError al Recibir el mensaje");
            break;
        }
        else
        {
            printf("\nCLIENTE: %s\n\n", super->paqRec);
        }
    }
    while(super->msj[0] != '\0');
}

void *Send(void *arg)
{
    struct superSocket *super = (struct superSocket*)arg;
    int i;
    do
    {
        i = 0;
        while((super->msj[i++] = getchar()) != '\n');
        super->msj[--i] = '\0';
        fflush(stdin);
        super->tam = sendto(super->udp_socket, super->msj, i, 0, (struct sockaddr *)&super->CLIENTE, sizeof(struct sockaddr_in));
        if(super->tam == -1)
        {
            perror("\nError al enviar el mensaje");
            break;
        }
    }
    while(super->msj[0] != '\0');
}

int main ( int argc, char *argv[] )
{
    pthread_t threadRecive, threadSend;
    struct superSocket super;

    super.udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(super.udp_socket == -1)
    {
        perror("\nError al Abrir el Socket");
        return 1;
    }
    else
    {
        perror("\nExito al Abrir el Socket");
    }
    
    super.SERVIDOR.sin_family = AF_INET;
    super.SERVIDOR.sin_port = htons(8080); //Host on Network Short = 8080 Puerto dinamico
    super.SERVIDOR.sin_addr.s_addr = INADDR_ANY;

    super.lbind = bind(super.udp_socket, (struct sockaddr *)&super.SERVIDOR, sizeof(super.SERVIDOR));
    if(super.lbind == -1)
    {
        perror("\nError al generar el Bind");
        return 1;
    }
    else
    {
        perror("\nExito al generar el Bind ");
    }
    printf("\n\n");
    fflush(stdin);
    if(pthread_create(&threadRecive, NULL, Recive, &super) != 0)
    {
        perror("\nNo se pudo crear el hilo de Recive");
        return 1;
    }
    fflush(stdin);
    if(pthread_create(&threadSend, NULL, Send, &super) != 0)
    {
        perror("\nNo se pudo crear el hilo de Send");
        return 1;
    }
    
    pthread_join(threadSend,NULL);

    printf("\n");
    close(super.udp_socket);
    return EXIT_SUCCESS;
}

