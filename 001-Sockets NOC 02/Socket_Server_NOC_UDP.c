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

int main ( int argc, char *argv[] )
{
    int udp_socket, lbind, tam, lrecv;
    int i = 0;
    unsigned char msj[512];
    unsigned char paqRec[512];
    struct sockaddr_in SERVIDOR, CLIENTE;
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(udp_socket == -1)
    {
        perror("\nError al Abrir el Socket");
        return 1;
    }
    else
    {
        perror("\nExito al Abrir el Socket");
    }
    
    SERVIDOR.sin_family = AF_INET;
    SERVIDOR.sin_port = htons(8080); //Host on Network Short = 8080 Puerto dinamico
    SERVIDOR.sin_addr.s_addr = INADDR_ANY;
    lbind = bind(udp_socket, (struct sockaddr *)&SERVIDOR, sizeof(SERVIDOR));
    
    if(lbind == -1)
    {
        perror("\nError al generar el Bind");
        return 1;
    }
    else
    {
        perror("\nExito al generar el Bind\n");
    }

    do
    {
        lrecv = sizeof(CLIENTE);
        tam = recvfrom(udp_socket, paqRec, 512, 0, (struct sockaddr *)&CLIENTE, &lrecv);
    
        if(tam == -1)
        {
            perror("\Error al Recibir el mensaje");
            return 1;
        }
        else
        {
            printf("\nEl mensaje: %s\n\n", paqRec);
        }

        i = 0;
        while((msj[i++] = getchar()) != '\n');
        msj[--i] = '\0';

        fflush(stdin);
        tam = sendto(udp_socket, msj, strlen(msj) + 1, 0, (struct sockaddr *)&CLIENTE, sizeof(CLIENTE));
    
        if(tam == -1)
        {
            perror("\nError al enviar el mensaje");
            return 1;
        }
        else
        {
            perror("\nExito al enviar el mensaje\n");
        }
    }
    while(msj[0] != '\0');
    
    printf("\n");
    close(udp_socket);
    return EXIT_SUCCESS;
}

