/*
 * =====================================================================================
 *
 *       Filename:  calculadora_ip.c
 *
 *    Description:  calculadora de IP's
 *
 *       Revision:  none
 *       Compiler:  gcc -o calip calculadora_ip.c
 *
 *         Author:  Juan Rene Hernandez Sanchez, jhernandezs1509@alumno.ipn.mx
 *   Organization:  ESCOM
 *
 * =====================================================================================
 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
int ClaseRed(unsigned char *);
void Tipo(int,FILE*);
void Subredes(unsigned char *,int);
void SubredesTipoA(unsigned char*,int,int,FILE *);
void SubredesTipoB(unsigned char*,int,int,FILE *);
void SubredesTipoC(unsigned char*,int,int,FILE *);
int SubredesTotales(int);
void ObtenerHost (unsigned char *,int,int,int,FILE *);
void MascaraRed(unsigned char * ,unsigned char *,int,int);
int HostTotales(int TotalHost);
void Host(unsigned char *,int);
void Prefijo(unsigned char *,int);

int main(){
    int opcion,clasered=-1;
    unsigned char IP[4];
    FILE *Archivo;
    printf("----------------------------Calculadora de Subredes------------------------------");
    do{
        Archivo = fopen ("Subredes.txt","w+");
        if(Archivo==NULL){
                fputs("Error al abrir el archivo",stderr);
                exit(1);
        }
        fclose(Archivo);
        printf("\nIngrese la IP:");
        scanf("%hhu.%hhu.%hhu.%hhu",IP,IP+1,IP+2,IP+3);
        clasered=ClaseRed(IP);
        printf("\n\tTipo: %d\n",clasered);
        
        printf("\n\t1.Subredes");
        printf("\n\t2.Host");
        printf("\n\t3.Prefijo");
        printf("\n\t4.Salir");
        printf("\n\tElija una opcion: ");
        scanf("%d",&opcion);
        while (opcion<0 || opcion>6){
            printf("\n**********Opcion invalida**********\n");
            printf("\nElija una opcion: ");
            scanf("%d",&opcion);
        }
        switch(opcion){
            case 1: Subredes(IP,clasered);
                    printf("\n\t4.Salir | 5.Otra red: ");
                    scanf("%d",&opcion);
                break;
            case 2:
                    Host(IP,clasered);
                    printf("\n\t4.Salir | 5.Otra red: ");
                    scanf("%d",&opcion);
                break;
            case 3: Prefijo(IP,clasered);
                    printf("\n\t4.Salir | 5.Otra red: ");
                    scanf("%d",&opcion);
                break;
            default:
                break;
        }
    }while(opcion!=4);
return 0;
}
int ClaseRed(unsigned char * ip){
    if ( ip[0] < 128 ){
        if ( ip[0] == 10 )
            return 4;
        else
            return 1;
     }
    if ( ip[0] == 192 && ip[1] == 168 )
                return 5;
    if ( (ip[0] > 127) ){
        if (ip[0] == 172 && (ip[1]&0x10) == 16)
            return 6;
        if (ip[0] < 192)
            return 2;
    }
    if ( (ip[0] > 191) ){
        if  (ip[0] < 224)
            return 3;
    }
}
void Tipo(int tipo,FILE* Archivo){
    printf("\nTipo de red correcto\n");
    if ( tipo==4 )
        fprintf(Archivo,"%s","\n\tLa red es clase A de Direccinamiento Privado\n");
    if ( tipo==5 )
        fprintf(Archivo,"%s","\n\tLa red es clase C de Direccinamiento Privado\n");
    if (tipo==6)
        fprintf(Archivo,"%s","\n\tLa red es clase B de Direccinamiento Privado\n");
    if(tipo==1)
        fprintf(Archivo,"%s","\n\tLa red es clase A\n");
    if ( tipo==2) 
        fprintf(Archivo,"%s","\n\tLa red es clase B\n");
    if ( tipo==3 )
        fprintf(Archivo,"%s","\n\tLa red es clase C\n");
}
void Subredes(unsigned char *IP, int tipo){
    int NSubredes,Nbits,NBitsHost,Prefijo;
    unsigned char Mascara[4];
    FILE * fp;
    fp = fopen ("Subredes.txt","r+");
    if( fp==NULL ){
        puts("Error en el manejo de archivos");
        exit(1);
    }
    printf("\n\tIngrese el numero de subredes: ");
    scanf("%d",&NSubredes);
    Nbits = ( (log(NSubredes) ) / ( log(2) ) ) + 1;
    if (tipo == 1 || tipo == 4){
        NBitsHost = 24 - ( Nbits );
        Prefijo=8+Nbits;
    }
    if (tipo == 2 || tipo == 6){
        NBitsHost = 16 - ( Nbits );
        Prefijo=16+Nbits;
    }
    if (tipo == 3 || tipo == 5){
        NBitsHost = 8 - ( Nbits );
        Prefijo=24+Nbits;
    }
    MascaraRed(IP,Mascara,Nbits,tipo);  
    fprintf(fp,"-----------RED: %d.%d.%d.%d-----------------",IP[0],IP[1],IP[2],IP[3]);
    fprintf(fp,"\n\tNumero de subredes: %d",SubredesTotales(Nbits));
    fprintf(fp,"\n\tNumero de Host: %d",HostTotales(NBitsHost));
    fprintf(fp,"\n\tMascara de subred: %d.%d.%d.%d",Mascara[0],Mascara[1],Mascara[2],Mascara[3]);
    fprintf(fp,"\n\tPrefijo: /%d",Prefijo);
    Tipo(tipo,fp);
    if (tipo == 1 || tipo == 4 )
        SubredesTipoA(IP,Nbits,NBitsHost,fp);
    if (tipo == 2 || tipo == 6)
        SubredesTipoB(IP,Nbits,NBitsHost,fp);
    if (tipo == 3 || tipo == 5)
        SubredesTipoC(IP,Nbits,NBitsHost,fp);
    printf("\nListo checar 'Subredes.txt'\n");
    fclose(fp);
}
void SubredesTipoA(unsigned char *IP ,int BitsSubredes,int BitsHost,FILE * Archivo){
    int i,j,k;
    unsigned char Ip[4] = {0,0,0,0};
    if(BitsSubredes <= 8){
        fprintf(Archivo,"-------------Subredes---------------------");
        for(i=0 ; i < SubredesTotales(BitsSubredes) ; i++){
            IP[1] = (Ip[1]+i) << (8 - BitsSubredes);
            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
        }
    }
    if( BitsSubredes > 8 && BitsSubredes <= 16 ){
        fprintf(Archivo,"-------------Subredes---------------------");
        for(i=0 ; i < SubredesTotales(BitsSubredes) ; i++){
            IP[1] = (Ip[1]+i) >> ( 8 - (16 - BitsSubredes));
            IP[2] = (Ip[2]+i) << ( 16 - BitsSubredes);
            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
        }
    }
    if (  BitsSubredes > 16 && BitsSubredes <= 24 ) {
        fprintf(Archivo,"-------------Subredes---------------------");
        for(i=0 ; i < SubredesTotales(BitsSubredes) ; i++){
            IP[1] = (Ip[1]+i) >> ( 16 - (24 - BitsSubredes));
            IP[2] = (Ip[2]+i) >> ( 16 - (24 - BitsSubredes));
            IP[3] = (Ip[3]+i) << ( 24 - BitsSubredes);
            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
        }
    }
    ObtenerHost (IP,BitsHost,BitsSubredes,1,Archivo);
}
void SubredesTipoB(unsigned char *IP ,int BitsSubredes,int BitsHost,FILE * Archivo){
    int i,j,k;
    unsigned char Ip[4] = {0,0,0,0};
    if(BitsSubredes <= 8){
        fprintf(Archivo,"-------------Subredes---------------------");
        for(i=0 ; i < SubredesTotales(BitsSubredes) ; i++){
            IP[2] = ((Ip[2]+i) << (8 - BitsSubredes));
            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
        }
    }
    if( BitsSubredes > 8 && BitsSubredes <= 16 ){
        fprintf(Archivo,"-------------Subredes---------------------");
        for(i=0 ; i < SubredesTotales(BitsSubredes) ; i++){
            IP[2] = ( (Ip[2]+i) >> (8- (16 - BitsSubredes)) );
            IP[3] = ( (Ip[3]+i) << ( 16 - BitsSubredes) );
            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
        }
    }
    ObtenerHost (IP,BitsHost,BitsSubredes,2,Archivo);
}
void SubredesTipoC(unsigned char *IP ,int BitsSubredes,int BitsHost,FILE * Archivo){
    int i;
    unsigned char Ip[4] = {0,0,0,0};
    fprintf(Archivo,"-------------Subredes---------------------");
    for(i=0 ; i < SubredesTotales(BitsSubredes) ; i++){
        IP[3] = (Ip[3]+i) << (8 - BitsSubredes);
        fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
    }
    ObtenerHost (IP,BitsHost,BitsSubredes,3,Archivo);
}
void ObtenerHost (unsigned char * IP,int BitsHost,int BitsSubredes,int tipo,FILE *Archivo){
    int i,j,k,l,m;
    unsigned char Ip[4] = {0,0,0,0};
    fprintf(Archivo,"--------------Host------------------------");
    if(tipo==1){
        if(BitsSubredes <= 8){
            for(j=0 ; j < SubredesTotales(BitsSubredes) ; j++){
                IP[1] = ((Ip[1]+j) << (8 - BitsSubredes));
                for(k=1; k <= (HostTotales(8-BitsSubredes)+2) ; k++){
                    for(l=0; l < 255 ; l++){
                        IP[2] = (Ip[2]+l);
                        for( m=0; m < 255 ; m++){    
                            IP[3] = (Ip[3]+m);
                            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
                        }
                    }
                    IP[1] = (IP[1]+1);
                }
                fprintf(Archivo,"\n-----------------------------------------------------------------------\n");   
            }
        }
        if(BitsSubredes > 8 && BitsSubredes <=16){
            for(j=0 ; j < SubredesTotales(BitsSubredes) ; j++){
                IP[2] = ((Ip[2]+j) << (8 - BitsSubredes));
                    for(k=1; k <= (HostTotales(8-BitsSubredes)+2) ; k++){
                        for(l=0; l <= 255 ; l++){
                            IP[3] = (Ip[3]+l);
                            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
                        }
                        IP[2] = (IP[2]+1);
                    }
                fprintf(Archivo,"\n-----------------------------------------------------------------------\n");   
            }
        }
        if(BitsSubredes > 16 && BitsSubredes <=24){
            for(j=0 ; j < SubredesTotales(BitsSubredes) ; j++){
                IP[3] = (Ip[3]+j) << (8 - BitsSubredes);
                for(i=1 ; i <  HostTotales(BitsHost) ; i++){
                    fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
                    IP[3] = (IP[3]+1);
                }
                fprintf(Archivo,"\n-----------------------------------------------------------------------\n");
            }
        }
    }
    if(tipo==2){
        if(BitsSubredes <= 8){
            for(j=0 ; j < SubredesTotales(BitsSubredes) ; j++){
                IP[2] = ((Ip[2]+j) << (8 - BitsSubredes));
                    for(k=1; k <= (HostTotales(8-BitsSubredes)+2) ; k++){
                        for(l=0; l <= 255 ; l++){
                            IP[3] = (Ip[3]+l);
                            fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
                        }
                        fprintf(Archivo,"\n***************************************\n"); 
                        IP[2] = (IP[2]+1);
                    }
                fprintf(Archivo,"\n------------------------------------------\n");   
            }
        }
        if(BitsSubredes > 8 && BitsSubredes <=16){
            for(j=0 ; j < SubredesTotales(BitsSubredes) ; j++){
                IP[3] = (Ip[3]+j) << (8 - BitsSubredes);
                for(i=-1 ; i <  HostTotales(BitsHost) ; i++){
                    fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
                    IP[3] = (IP[3]+1);
                }
                fprintf(Archivo,"\n------------------------------------------\n");
            }
        }
    }
    if(tipo==3){
        for(j=0 ; j < SubredesTotales(BitsSubredes) ; j++){
            IP[3] = (Ip[3]+j) << (8 - BitsSubredes);
            for(i=1 ; i <  HostTotales(BitsHost) ; i++){
                fprintf(Archivo,"\n\t %d.%d.%d.%d\n",IP[0],IP[1],IP[2],IP[3]);
                IP[3] = (IP[3]+1);
            }
            fprintf(Archivo,"\n------------------------------------------\n");
        }
    }
}
int SubredesTotales(int TotalSubredes){
    return pow(2,TotalSubredes)-2;
    //return pow(2,TotalSubredes);

}
void MascaraRed(unsigned char *IP ,unsigned char *Mascara ,int Subredes,int tipo){
    unsigned char aux1[4]={255,255,255,255},aux0[4]={0,0,0,0};
    if (tipo == 1 || tipo == 4 || tipo == 5 || tipo == 6){
        Mascara[0]=aux1[0];
        if(Subredes<=8){
            Mascara[1] = ( aux1[1] << (8 - Subredes) );
            Mascara[2]=aux0[2];
            Mascara[3]=aux0[3];
        }
        if(Subredes>8 && Subredes <=16){
            Mascara[0]=aux1[0];
            Mascara[1] = ( aux1[1] );
            Mascara[2] = ( aux1[2] << (16 - Subredes) );
            Mascara[3] = aux0[3];
        }
        if(Subredes>16 && Subredes<=24){
            Mascara[0]=aux1[0];
            Mascara[1] = ( aux1[1] );
            Mascara[2] = ( aux1[2] );
            Mascara[3] = ( aux1[3] << (16 - Subredes) );
        }
    }
    if (tipo == 2){
        Mascara[0]=aux1[0];
        Mascara[1]=aux1[1];
        if(Subredes<=8){
            Mascara[2]=( aux1[2] << (8 - Subredes) );
            Mascara[3]=aux0[3];
        }
        if(Subredes>8 && Subredes <=16){
            Mascara[2] = ( aux1[2] );
            Mascara[3] = ( aux1[3] << (16 - Subredes) );
        }
    }
    if (tipo == 3){
        Mascara[0]=aux1[0];
        Mascara[1]=aux1[1];
        Mascara[2]=aux1[2];
        Mascara[3]= ( aux1[3] << (8 - (Subredes)) );
    }
}
int HostTotales(int TotalHost){
    return pow(2,TotalHost)-2;
    //return pow(2,TotalHost);

}
void Host(unsigned char *IP,int tipo){
    int NHost,Nbits,NBitSubredes,Prefijo;
    unsigned char Mascara[4];
    FILE * fp;
    fp = fopen ("Subredes.txt","r+");
    if( fp==NULL ){
        puts("Error en el manejo de archivos");
        exit(1);
    }
    printf("\n\tIngresa el numero de Host: ");
    scanf("%d" , &NHost);
    Nbits = ( (log( NHost) ) / ( log(2) ) ) + 1;
    //printf("\n\tBits prestados: %d",Nbits);
    if (tipo == 1 || tipo == 4){
        NBitSubredes = 24 - ( Nbits );
        Prefijo=8+NBitSubredes;
    }
    if (tipo == 2 || tipo == 6){
        NBitSubredes = 16 - ( Nbits );
        Prefijo=16+NBitSubredes;
    }
    if (tipo == 3 || tipo == 5){
        NBitSubredes = 8 - ( Nbits );
        Prefijo=24+NBitSubredes;
    }
    MascaraRed(IP,Mascara,NBitSubredes,tipo);
    fprintf(fp,"-----------RED: %d.%d.%d.%d-----------------",IP[0],IP[1],IP[2],IP[3]);
    fprintf(fp,"\n\tNumero de subredes: %d",SubredesTotales(NBitSubredes));
    fprintf(fp,"\n\tNumero de Host: %d",HostTotales(Nbits));
    fprintf(fp,"\n\tMascara de subred: %d.%d.%d.%d",Mascara[0],Mascara[1],Mascara[2],Mascara[3]);
    fprintf(fp,"\n\tPrefijo: /%d",Prefijo);
    Tipo(tipo,fp);
    if (tipo == 1 || tipo == 4  )
        SubredesTipoA(IP,NBitSubredes,Nbits,fp);
    if (tipo == 2 || tipo == 6)
        SubredesTipoB(IP,NBitSubredes,Nbits,fp);
    if (tipo == 3 || tipo == 5)
        SubredesTipoC(IP,NBitSubredes,Nbits,fp);
    printf("\nListo checar 'Subredes.txt'\n");
    fclose(fp);
}
void Prefijo(unsigned char *IP,int tipo){
    int Prefijo,NBitsHost,NBitSubredes,PrefijoValido=-1;;
    unsigned char Mascara[4];
    FILE * fp;
    fp = fopen ("Subredes.txt","r+");
    if( fp==NULL ){
        puts("Error en el manejo de archivos");
        exit(1);
    }
    printf("\n\tIngresa el prefijo: ");
    scanf("%d" , &Prefijo);
    /*do{
        if (tipo == 1 || tipo == 4  ){
            if(Prefijo>8 && Prefijo<30)
        }
        if (tipo == 2 || tipo == 6){
            f(Prefijo>16 && Prefijo<30)
        }
        if (tipo == 3 || tipo == 5){
            f(Prefijo>24 && Prefijo<30)
        }
    }while(PrefijoValido==-1);*/
    
    if (tipo == 1 || tipo == 4  )
        NBitSubredes = 24 - ( Prefijo );
    if (tipo == 2 || tipo == 6)
        NBitSubredes = 16 - ( Prefijo );
    if (tipo == 3 || tipo == 5)
        NBitSubredes = 8 - ( Prefijo );
    
    if (tipo == 1 || tipo == 4  )
        NBitsHost = 24 - ( NBitSubredes );
    if (tipo == 2 || tipo == 6)
        NBitsHost = 16 - ( NBitSubredes );
    if (tipo == 3 || tipo == 5)
        NBitsHost = 8 - ( NBitSubredes );
    
    MascaraRed(IP,Mascara,NBitSubredes,tipo);
    fprintf(fp,"-----------RED: %d.%d.%d.%d-----------------",IP[0],IP[1],IP[2],IP[3]);
    fprintf(fp,"\n\tNumero de subredes: %d",SubredesTotales(NBitSubredes));
    fprintf(fp,"\n\tNumero de Host: %d",HostTotales(NBitsHost));
    fprintf(fp,"\n\tMascara de subred: %d.%d.%d.%d",Mascara[0],Mascara[1],Mascara[2],Mascara[3]);
    fprintf(fp,"\n\tPrefijo: /%d",Prefijo);
    Tipo(tipo,fp); 
    if (tipo == 1 || tipo == 4  )
        SubredesTipoA(IP,NBitSubredes,NBitsHost,fp);
    if (tipo == 2 || tipo == 6)
        SubredesTipoB(IP,NBitSubredes,NBitsHost,fp);
    if (tipo == 3 || tipo == 5)
        SubredesTipoC(IP,NBitSubredes,NBitsHost,fp);
    printf("\nListo checar 'Subredes.txt'\n");
    fclose(fp);
}