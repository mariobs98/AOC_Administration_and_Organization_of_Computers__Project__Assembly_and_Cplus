/***************************************************************************
 *   Copyright (C) 2018 by pilar   *
 *   pilarb@unex.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "imageprocess.h"


void imageprocess::rotar(uchar * imgO, uchar * imgD, float angle)

{
    short cw;
    int sin1000, cos1000;
    float seno, coseno; //%6  %7
    int aux;    //%8

    sin1000 = sin(angle)*1000.;
    cos1000 = cos(angle)*1000.;

    asm volatile(

        "fstcw %3\n\t"
        "fwait\n\t"
        "mov %3, %%ax\n\t"
        "and $0xf3ff, %%ax\n\t"
        "or $0x0c00, %%ax\n\t"
        "push %%rax\n\t"
        "fldcw (%%rsp)\n\t"
        "pop %%rax\n\t"

        "mov %0, %%rsi;"   //rsi contiene la direccion de la img origen
        "mov %1, %%rdi;"   //rdi contiene la direccion de la img destino

        "flds %2;"       //Carga en fpu directamente de memoria el angulo
        "fsin;"          //Seno del angulo
        "fstps %6;"      //Seno del angulo en %6

        "flds %2;"       //Carga en fpu directamente de memoria el angulo
        "fcos;"          //Coseno del angulo
        "fstps %7;"     //Coseno del angulo en %7


        "mov $0, %%r8;"    //fD---r8 contador de filas, puesta a 0
        "BucleFilasR:"

        "mov $0, %%r9;"    //cD---r9 contador de columnas, puesta a 0
        "BucleColumnasR:"

        "mov %%r9, %%rax;"
        "sub $400, %%rax;"  //cD-400 en rax
        "mov %%eax,%8;"    // eax en aux(%10) en memoria
        "fildl %8;"         //Carga cD-400 en fpu
        "fmuls %6;"       //SENO*(cD-400)---(%6*%10)

        "mov %%r8, %%rbx;"
        "sub $400, %%rbx;"   //fd-400 en rbx
        "mov %%ebx, %8;"   //ebx a variable aux en memoria(%10)
        "fildl %8;"        //Carga fD-400 en fpu
        "fmuls %7;"       //COSENO*(fD-400)---(%7*%10)

        "faddp %%st(0), %%st(1) ;"       //SENO*(cD-400)+COSENO*(fD-400)
        "fistpl %8;"       //Todo lo anterior en %10(aux)

        "mov %8,%%ecx;"
        "movsx %%ecx, %%rcx;"
        "add $240, %%rcx;"   //rcx = f0+240   == f0



        "mov %%ebx, %8;"
        "fildl %8;"
        "fmuls %6;"

        "mov %%eax, %8;"
        "fildl %8;"
        "fmuls %7;"    //COSENO*(cD-400)
        "fsubp %%st(0), %%st(1);"   //COSENO*(cD-400)-SENO(fD-400)
        "fistpl %8;"   //Lo anterior a Aux

        "mov %8, %%edx;"
        "movsx %%edx, %%rdx;"
        "add $320, %%rdx;"   //rdx=c0+320    == c0


        "cmp $0, %%rcx;"    //Comprobaciones del IF
        "jl ElseR;"
        "cmp $480, %%rcx;"
        "jge ElseR;"
        "cmp $0, %%rdx;"
        "jl ElseR;"
        "cmp $640, %%rdx;"
        "jge ElseR;"


        "imul $640, %%rcx;"
        "add %%rcx, %%rdx;"   //rdx indice0
        "mov $0, %%rax;"
        "mov (%%rdx,%%rsi),%%al;"   //[dirDest]=[dirOrig + indice0]
        "mov %%al,(%%rdi);"
        "jmp finBucleC;"


        "ElseR:"

        "movb $0, (%%rdi);"  //SINO

        "finBucleC:"

        "inc %%rdi;"    //Incremento direccion destino
        "inc %%r9;"    //Incremento contador de columnas
        "cmp $800, %%r9;"  //Comparo cont.de columnas si es menor de 800
        "jl BucleColumnasR;"    //En caso afirmativo salto a BucleColumnasR para repetir el bucle
        "inc %%r8;"        //Incremento contador de filas
        "cmp $800, %%r8;"  //Comparo cont.de filas si es menor de 800
        "jl BucleFilasR;"     //En caso afirmativo salto a BuclefilasR para repetir el bucle

        "fldcw %3\n\t"

        :
        : "m" (imgO), "m" (imgD), "m" (angle), "m" (cw), "m" (sin1000), "m" (cos1000), "m" (seno), "m" (coseno), "m" (aux)
        : "%r8", "%r9", "%r10", "%r11", "%r12", "%rax", "%rbx", "%rcx", "%rdx", "%rsi", "%rdi", "memory"
    );
}




void imageprocess::zoom(uchar * imgO, uchar * imgD, float s, int dx, int dy)
{

    short cw;
    int sInt;
    int ampliar;
    int Aux; //%8

    if(s>=1)
    {
        sInt = s;
        ampliar = 1;
    }
    else
    {
        sInt = rint(1./s);
        ampliar = 0;
    }

    asm volatile(
        "fstcw %5\n\t"
        "fwait\n\t"
        "mov %5, %%ax\n\t"
        "and $0xf3ff, %%ax\n\t"
        "or $0x0c00, %%ax\n\t"
        "push %%rax\n\t"
        "fldcw (%%rsp)\n\t"
        "pop %%rax\n\t"

        "mov %0, %%rsi;"   //rsi contiene la direccion de la img origen
        "mov %1, %%rdi;"   //rdi contiene la direccion de la img destino


        "mov $0, %%r8;"    //fD---r8 contador de filas, puesta a 0
        "BucleFilasZ:"

        "mov $0, %%rax;"
        "mov %4, %%eax;"    //eax contiene dy
        "add %%r8, %%rax;"  //Suma de fD+dy en rax
        "mov %%eax, %8;"    //eax a Aux
        "fildl %8;"         //Carga en fpu de fD+dy
        "fdivs %2;"         //fD+dy/s;
        "fistpl %8;"        //Lo anterior a Aux
        "mov %8, %%ecx;"    //ecx = (fD+dy)/s    = f0
        "movsx %%ecx, %%rcx;"


        "mov $0, %%r9;"    //cD---r9 contador de columnas, puesta a 0
        "BucleColumnasZ:"

        "mov $0, %%rax;"
        "mov %3, %%eax;"    //rax contiene dx
        "add %%r9, %%rax;"  //Suma de cD+dx en rax
        "mov %%eax, %8;"    //eax a Aux
        "fildl %8;"         //Carga en fpu de cD+dx
        "fdivs %2;"         //cD+dx/s;
        "fistpl %8;"        //Lo anterior a Aux
        "mov %8, %%edx;"    //rdx = (cD+dx)/s    = c0
        "movsx %%edx, %%rdx;"


        "cmp $0, %%rcx;"    //Comprobaciones del IF
        "jl ElseZ;"
        "cmp $800, %%rcx;"
        "jge ElseZ;"
        "cmp $0, %%rdx;"
        "jl ElseZ;"
        "cmp $800, %%rdx;"
        "jge ElseZ;"

        "mov %%rcx, %%r10;"  //f0 a r10
        "imul $800, %%r10;"   //f0*800
        "mov %%rdx, %%r11;"
        "add %%r10, %%r11;"   //r11 indice0  == f0*800+c0
        "mov $0, %%rax;"
        "mov (%%r11,%%rsi), %%al;"  //[dirDest]=[dirOrig + indice0]
        "mov %%al, (%%rdi);"
        "jmp finBucleZ;"


        "ElseZ:"

        "movb $0, (%%rdi);"  //SINO

        "finBucleZ:"

        "inc %%rdi;"    //Incremento direccion destino
        "inc %%r9;"     //Incremento contador de columnas
        "cmp $640, %%r9;"  //Comparo cont.de columnas si es menor de 640
        "jl BucleColumnasZ;"    //En caso afirmativo salto a BucleColumnasZ para repetir el bucle
        "inc %%r8;"        //Incremento contador de filas
        "cmp $480, %%r8;"  //Comparo cont.de filas si es menor de 480
        "jl BucleFilasZ;"     //En caso afirmativo salto a BuclefilasZ para repetir el bucle

        "fldcw %5\n\t"

        :
        : "m" (imgO), "m" (imgD), "m" (s), "m" (dx), "m" (dy), "m" (cw), "m" (sInt), "m" (ampliar), "m" (Aux)
        : "%r8", "%r9", "%r10", "%r11", "%rax", "%rcx", "%rdx", "%rsi", "%rdi", "memory"
    );
}



void imageprocess::volteoHorizontal(uchar * imgO, uchar * imgD)
{
    asm volatile(
        "\n\t"

        "mov %0, %%rsi;"    //rsi contiene direccion de inicio de la img origen
        "mov %1, %%rdi;"    //rdi contiene direccion de inicio de la img destino
        "add $639, %%rsi;"  //nos colocamos al final de la primera fila en la img origen
        "mov $0, %%rax;"    //pongo a 0 rax para usarlo como almacenamiento del pixel a copiar
        "mov $0, %%edx;"    //edx contador de filas, puesta a 0
        "BucleFilasH:"

        "mov $0, %%ecx;"    //ecx contador de columnas,puesta a 0
        "BucleColumnasH:"
        "mov (%%rsi), %%al;" //Asigancion pixel origen a pixel destino
        "mov %%al, (%%rdi);"
        "dec %%rsi;"        //Decremento en uno la columna de la img original
        "inc %%rdi;"        //Incremento en uno la columna de la img destino
        "inc %%ecx;"        //Incremento contador de columnas
        "cmp $640,%%ecx;"   //Comparo si ya he recorrido todas las columnas
        "jl BucleColumnasH;" //salto si menor a BucleColumnas
        "add $1280, %%rsi;" //sumo 1280 a img origen para pasar al final de la siguiente fila a copiar
        "inc %%edx;"        //Incremento la fila
        "cmp $480, %%edx;"  //Comparo si ya he reocrrido todas las filas
        "jl BucleFilasH;"    //En caso afirmativo, repetimos haciendo un salto si menor a BucleFilas

        :
        : "m" (imgO),	"m" (imgD)
        :  "rax", "%ecx", "%edx", "%rsi", "%rdi", "memory"
    );
}




void imageprocess::volteoVertical(uchar * imgO, uchar * imgD)
{
    asm volatile(
        "\n\t"

        "mov %0, %%rsi;"    //rsi contiene direccion de inicio de la img origen
        "mov %1, %%rdi;"    //rdi contiene direccion de inicio de la img destino
        "mov $640, %%rax;"
        "imul $479, %%rax;"  //multiplicacion de 479*640
        "add %%rax, %%rsi;"  //nos colocamos al final del todo de la img origen
        "mov $0, %%rax;"    //pongo a 0 rax para usarlo como almacenamiento del pixel a copiar
        "mov $0, %%edx;"    //edx contador de filas, puesta a 0
        "BucleFilasV:"

        "mov $0, %%ecx;"    //ecx contador de columnas,puesta a 0
        "BucleColumnasV:"
        "mov (%%rsi), %%al;" //Asigancion pixel origen a pixel destino
        "mov %%al, (%%rdi);"
        "inc %%rsi;"        //Incremento en uno la columna de la img original
        "inc %%rdi;"        //Incremento en uno la columna de la img destino
        "inc %%ecx;"        //Incremento contador de columnas
        "cmp $640,%%ecx;"   //Comparo si ya he recorrido todas las columnas
        "jl BucleColumnasV;" //salto si menor a BucleColumnas
        "sub $1280, %%rsi;" //resto 1280 a img origen para colocarme en la fila previa al final de la misma
        "inc %%edx;"        //Incremento la fila
        "cmp $480, %%edx;"  //Comparo si ya he reocrrido todas las filas
        "jl BucleFilasV;"    //En caso afirmativo, repetimos haciendo un salto si menor a BucleFilas

        :
        : "m" (imgO),	"m" (imgD)
        : "rax", "%ecx", "%edx", "%rsi", "%rdi", "memory"
    );
}



void imageprocess::iluminarLUT(uchar * tablaLUT, uchar gW)
{
    asm volatile(
        "\n\t"

        "mov %0, %%rsi;"    //rsi contiene direccion de tablaLUT
        "mov $0, %%r9;"
        "mov %1, %%r9b;"    //r9b contiene la direccion de gW

        "mov $0, %%r8;"     //0 a r8(g), contador del primer bucle
        "BucleI1:"

        "cmp %%r9, %%r8;"
        "jge BucleI2;"

        "mov $0, %%rax;"

        "imul $255, %%r8, %%rax;" //g*255 en rax;
        "mov $0, %%rdx;"
        "divb %%r9b;"   //(g*255)/gW   //cociente en rax/ resto en rdx
        "movb %%al, (%%rsi,%%r8);"

        "inc %%r8;"         //Comprobacion de bucle
        "jmp BucleI1;"

        "BucleI2:"

        "cmp $256, %%r8;"
        "jge BucleIFin;"

        "movb $255, (%%rsi,%%r8);"

        "inc %%r8;"         //Comprobacion de bucle
        "jmp BucleI2;"

        "BucleIFin:"
        :
        : "m" (tablaLUT), "m" (gW), "m" (gW)
        : "%r8", "%r9", "%rax", "%rsi", "memory"
    );        
}



void imageprocess::oscurecerLUT(uchar * tablaLUT, uchar gB)
{
    asm volatile(
        "\n\t"

        "mov %0, %%rsi;"    //rsi contiene direccion de tablaLUT
        "mov $0, %%r9;"
        "mov %1, %%r9b;"    //r9b contiene la direccion de gB

        "mov $0, %%r8;"     //0 a r8(g), contador del primer bucle
        "BucleO1:"

        "cmp %%r9, %%r8;"
        "jg BucleO2;"

        "movb $0, (%%rsi,%%r8);"

        "inc %%r8;"         //Comprobacion de bucle
        "jmp BucleO1;"

        "BucleO2:"

        "cmp $256, %%r8;"
        "jge BucleOFin;"

        "mov $0, %%rax;"
        "mov $0, %%rbx;"

        "movb %%r8b, %%al;"
        "sub %%r9b, %%al;"
        "imul $255, %%rax;"  //rax = (g-gB)*255

        "movb $255, %%bl;"
        "sub %%r9b, %%bl;"  //rbx = (255-gB)

        "divb %%bl;"    //cociente en rax;

        "movb %%al,(%%rsi,%%r8);"

        "inc %%r8;"         //Comprobacion de bucle
        "jmp BucleO2;"

        "BucleOFin:"
        :
        : "m" (tablaLUT), "m" (gB)
        : "%r8", "%r9", "%rax", "%rbx", "%rsi", "memory"
    );
}



void imageprocess::iluminarLUTMejorado(uchar * tablaLUT, uchar gW)
{
    asm volatile(
        "\n\t"


        :
        : "m" (tablaLUT), "m" (gW)
        : "memory"
    );

}



void imageprocess::oscurecerLUTMejorado(uchar * tablaLUT, uchar gB)
{
    asm volatile(
        "\n\t"


        :
        : "m" (tablaLUT), "m" (gB)
        : "memory"
    );

}



void imageprocess::aplicarTablaLUT(uchar * tablaLUT, uchar * imgO, uchar * imgD)
{
    asm volatile(
        "\n\t"

        "mov %1, %%r8;"     //r8 contiene la direccion de la tablaLUT
        "mov %0, %%rsi;"    //rsi contiene la direccion de la img origen
        "mov %2, %%rdi;"   //rdi contiene la direccion de la img destino

        "mov $640, %%rax;"
        "imul $480, %%rax;"        //480*640 en rax

        "mov %%rax, %%rdx;"

        "mov $0, %%rax;"    //Pongo a 0 registros para borrar basura
        "mov $0, %%rbx;"
        "mov $0, %%rcx;"    //Contador de bucle
        "Bucle:"

        "movb (%%rsi), %%al;"
        "movb (%%r8,%%rax), %%bl;"
        "movb %%bl, (%%rdi);"

        "inc %%rsi;"
        "inc %%rdi;"
        "inc %%rcx;"

        "cmp %%rdx, %%rcx;"
        "jl Bucle;"

        :
        : "m" (imgO), "m" (tablaLUT), "m" (imgD)
        : "%rax", "%rbx", "%rdx", "%rcx", "%rsi", "%rdi", "%r8","memory"
    );
}

