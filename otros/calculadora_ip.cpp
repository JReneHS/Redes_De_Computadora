/*
 * =====================================================================================
 *
 *       Filename:  calculadora_ip.cpp
 *
 *    Description:  calculadora de IP's
 *
 *        Version:  1.0
 *        Created:  30/01/21 22:26:12
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Juan Rene Hernandez Sanchez, jhernandezs1509@alumno.ipn.mx
 *   Organization:  ESCOM
 *
 * =====================================================================================
 */
#include <stdio.h>

struct ip_data {
  unsigned char ip[4];
  unsigned char m_red[4];
  unsigned char m_sub_red[4];
  char clase;
  unsigned char prefijo;
  unsigned int subredes;
  unsigned int host;

  ip_data() {
    ip = m_red = m_sub_red = {0,0,0,0};
    clase = 'A';
    prefijo = subredes = host = 0;
  }

//Aqui van los metodos
}

int main(int argc, char const *argv[]) {

  cout<<"ingrere la ip a calcular"<<endl;
  for(auto &x:ip)cin>>x;
  if(ip[0]<128){
    
  }else if()
  return 0;
}


