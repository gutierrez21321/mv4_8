#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"

//2 operandos

void setCC(int a){ //setea bit de signo
  int reg = registros[CC];
  if(a == 0){
    reg = reg & 0x000000001;
  }else{
    reg = reg & 0x000000000;
  }
  if(a < 0){
    reg = reg & 0x800000001;
  }else{
    reg = reg & 0x000000001;
  }
  registros[CC] = reg;
}

void modificarRegistro(int a,int valor){ //testear
  int esp,aux,reg;
  esp = a & 0xf;  //accedo registro
  aux = (a >> 4) & 0x3;  //sector de registro
  switch(aux){
      case 0: registros[esp] = registros[esp] & (~E);
              reg = valor;
              break;
      case 1: registros[esp] = registros[esp] & (~L);
              reg = valor & 0x000f;
              break;
      case 2: registros[esp] = registros[esp] & (~H);
              reg = (valor & 0x000f)<<8;
              break;
      case 3: registros[esp] = registros[esp] & (~X);
              reg = valor & 0x00ff;
              break;
    }
  registros[esp] = registros[esp] | reg;
}

int valor(int tipo,int operador){ //podria ser void y modificar operador//devuelve el valor para operar
  int valor,aux;
  if(tipo==1 && operador>=10 && operador<=16){ //registro
    aux = operador & 0xf; //accedo registro
    valor = registros[aux];

    aux = (operador >> 4) & 0x3; //sector de registro

    switch(aux){
      case 0: valor = valor & X;
              break;
      case 1: valor = valor & L;
              break;
      case 2: valor = (valor & H)>>8;
              break;
      case 3: valor = valor & X;
              break;
    }

  }else if(tipo==2) //memoria
    valor = memoria[registros[DS] + operador];
  else //inmediato
    valor = operador;
  return valor;
}

void mov(int opA,int a,int opB,int b){
  int valorB;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
   valorB = valor(opB,b);
   if(opA==1)//registro
     modificarRegistro(a,valorB);
   else//memoria
     memoria[registros[DS]+a] = valorB;
  }
}

void add(int opA,int a,int opB,int b){
  int suma;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    suma = valor(opA,a) + valor(opB,b);
    if(opA==1)  //registro
      modificarRegistro(a,suma);
    else //memoria
      memoria[registros[DS+a]] = suma;
    setCC(suma);
  }
}

void sub(int opA,int a,int opB,int b){
  int resta;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    resta = valor(opA,a) - valor(opB,b);
    if(opA==1)  //registro
      modificarRegistro(a,resta);
    else //memoria
      memoria[registros[DS+a]] = resta;
    setCC(resta);
  }
}

void mul(int opA,int a,int opB,int b){
  int multiplicacion;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    multiplicacion = valor(opA,a) * valor(opB,b);
    if(opA==1)  //registro
      modificarRegistro(a,multiplicacion);
    else //memoria
      memoria[registros[DS+a]] = multiplicacion;
    setCC(multiplicacion);
  }
}

void division(int opA,int a,int opB,int b){
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    div_t resultado = div(valor(opA,a),valor(opB,b));
    if(opA==1) //registro
      modificarRegistro(a,resultado.quot); //resultado.quot cociente entero
    else      //memoria
      memoria[registros[DS+a]] = resultado.quot;
    registros[AC] = abs(resultado.rem);    //resto entero positivo
    setCC(resultado.quot);
  }
}

void swap(int opA,int a,int opB,int b){
  int aux,valorB;
  if(opA!=3 && opA!=0 && opB!=3 && opB!=0){//si es valido
    aux = valor(opA,a);
    valorB = valor(opB,b);
    if(opA==1){   //registro
      modificarRegistro(a,valorB);
    }else  //memoria
      memoria[registros[DS+a]] = valorB;

    if(opB==1){   //registro
      modificarRegistro(b,aux);
    }else  //memoria
      memoria[registros[DS+b]] = aux;
  }
}

void cmp(int opA,int a,int opB,int b){ //a puede ser inmediato?? aca supongo que si
  int aux;
  if(opA!=3 && opB!=3)
    aux = valor(opA,a) - valor(opB,b);
  setCC(aux);
}

void and(int opA,int a,int opB,int b){
  int andBit;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    andBit = valor(opA,a) & valor(opB,b);
    if(opA==1) //registro
      modificarRegistro(a,andBit);
    else       //memoria
      memoria[registros[DS+a]] = andBit;
  }
  setCC(andBit);
}

void or(int opA,int a,int opB,int b){
  int orBit;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    orBit = valor(opA,a) & valor(opB,b);
    if(opA==1) //registro
      modificarRegistro(a,orBit);
    else       //memoria
      memoria[registros[DS+a]] = orBit;
  }
  setCC(orBit);
}

void xor(int opA,int a,int opB,int b){
  int xorBit;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    xorBit = valor(opA,a) & valor(opB,b);
    if(opA==1) //registro
      modificarRegistro(a,xorBit);
    else       //memoria
      memoria[registros[DS+a]] = xorBit;
  }
  setCC(xorBit);
}

void shl(int opA,int a,int opB,int b){
  int corrimiento;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    corrimiento = valor(opA,a) << valor(opB,b);
    if(opA==1)  //registro
      modificarRegistro(a,corrimiento);
    else        //memoria
      memoria[registros[DS+a]] = corrimiento;
  }
  setCC(corrimiento);
}

void shr(int opA,int a,int opB,int b){
  int corrimiento;
  if(opA!=3 && opA!=0 && opB!=3){//si es valido
    corrimiento = valor(opA,a) >> valor(opB,b);
    if(opA==1) //registro
      modificarRegistro(a,corrimiento);
    else    //memoria
      memoria[registros[DS+a]] = corrimiento;
  }
  setCC(corrimiento);
}

//1 operando

void read(){
  char car,lectura[MAXSTRING],binario[MAXBINARIO];
  int i,ingreso,conf,base;
  int alm = registros[0xE] & E; //almaceno desde posicion en memoria que esta en registro EDX
  conf = registros[0xA] & X;
  if((conf>>8) & 0x0001 == 0){ //interpreta dsp del enter
    for(i=0;i<registros[0xC]&X;i++){
      itoa(alm, binario, 2); //convierte la memoria en un string de ese numero en binario
      if((conf>>11) & 0x0001 == 0)) //escribe el prompt
        printf("[%s"+ binario + "]: "); //alm en binario
      scanf("%d",&ingreso); //leo
      printf("\n");
      if((conf>>3) & 0x0001 == 1) //hexa
        base = 16;
      else if((conf>>2) & 0x0001 == 1) //octal
        base = 8;
      else if(conf & 0x0001 == 1) //decimal
        base = 10;
      itoa(alm, cadena, base);
      memoria[alm++] = stringToInt(cadena,4BYTES);
    }
  }else{ //lee caracter a caracter dsp del enter
    i = 0;
    itoa(alm, binario, 2);
    if((conf>>11) & 0x0001 == 0)) //escribe el prompt
        printf("[%s" + binario + "]: "); //alm a binario
    scanf("%s",lectura);
    car = lectura[i]; //caracter
    while(car!=NULL && i<registros[0xC]&X){
      memoria[alm++] = car;
      car = lectura[++i];
    }
    memoria[alm] = 0x00000000;
  }
}

void write(){
  char almBinario,num[MAXSTRING];
  int prompt,endline,i,escribe;
  int alm = registros[0xE] & E; //leo desde posicion en memoria que esta en registro EDX
  conf = registros[0xA] & X;
  endline = (conf>>8) &0x0001; //0 agregar endline
  prompt = (conf>>11) & 0x0001;//agregar prompt
  for(i=0;i<i<registros[0xC]&X;i++){
   itoa(almBinario,alm,2);    //convierte la memoria en un string de ese numero en binario
   escribe = memoria[alm++];
   if((conf>>4) & 0x0001 == 1){ //caracter
    escribe = escribe & 0x000000ff; //byte menos significativo
    if(escribe<32 || escribe>126)  //ascii no imprimible
      escribe = '.';
    if(prompt) printf("[%s"+almBinario+"]");
    printf("%c",escribe);
    if(endline) printf("\n");
   }
   if((conf>>3) & 0x0001 == 1){ //hexadecimal
    itoa(escribe,num, 16);
    if(prompt) printf("[%s"+almBinario+"]");
    printf("%s%",num);
    if(endline) printf("\n");
   }
   if((conf>>2) & 0x0001 == 1){ //octal
    itoa(escribe,num,8);
    if(prompt) printf("[%s"+almBinario+"]");
    printf("%s@",num);
    if(endline) printf("\n");
   }
   if(conf& 0x0001 == 1)  //decimal
    if(prompt) printf("[%s"+almBinario+"]");
    printf("%d",escribe);
    if(endline) printf("\n");
  }
}

void breakpoint(){

}

void sys(int opA,int a){ //supongo que el operandoA es inmediato
  switch(){
  if(a==0x000000001)//lectura
    read();
  else if(a==0x000000002) //escritura
    write();
  else if(a==0x00000000f) // breakpoint
    breakpoint();
}

void jmp(int opA,int a){
  if(opA!=3)//si es valido
    registros[IP] = valor(opA,a);
}

void jp(int opA,int a){
  if(opA!=3)//si es valido
    if(((registros[CC] & 0x800000000)>>31) == 0x0)
      registros[IP] = valor(opA,a);
}

void jn(int opA,int a){
  if(opA!=3)//si es valido
    if(((registros[CC] & 0x800000000)>>31) == 0x1)
      registros[IP] = valor(opA,a);
}

void jz(int opA,int a){
  if(opA!=3)//si es valido
    if(registros[CC] & 0x000000001 == 0x1)
      registros[IP] = valor(opA,a);
}

void jnz(int opA,int a){
  if(opA!=3)//si es valido
    if(registros[CC] & 0x800000000 == 0x0)
      registros[IP] = valor(opA,a);
}

void ldh(int opA,int a){
  int operando,aux;
  operando = valor(opA,a) & 0x00ff;
  aux = registros[CC] & 0x00ff;
  registros[CC] = (operando << 8) | aux;
}

void ldl(int opA,int a){
  int operando,aux;
  operando = valor(opA,a) & 0x00ff;
  aux = registros[CC] & 0xff00;
  registros[CC] = operando | aux;
}

void rnd(int opA,int a,int opB,int b){
  if(opA!=3 && opA!=0 && opB!=3){
    int random = rand() % valor(opB,b);
    if(opA==1)//registro
      modificarRegistro(a,random);
    else
      memoria[registros[DS+a]] = random;
  }
}

void not(int opA,int a){
  int negacion;
  if(opA!=3 && opA!=0){
    negacion = ~valor(opA,a);
    if(opA==1) //registros
      modificarRegistro(a,negacion);
    else //memoria
      memoria[registros[DS+a]] = negacion;
  }
  setCC(negacion);
}

//0 operandos

void stop(){ //detiene programa
  registros[IP] == registros[DS];
}

