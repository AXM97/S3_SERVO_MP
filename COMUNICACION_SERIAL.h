/* 
 * File:   COMUNICACION_SERIAL.h
 * Author: WIN8
 *
 * Created on 21 de noviembre de 2015, 11:47 AM
 */

#ifndef COMUNICACION_SERIAL_H
#define	COMUNICACION_SERIAL_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdlib.h>
void INICIALIZA_COMUNICACION_SERIAL9600(void);
void ENVIAR_DATO(char DATO);
void ENVIAR_CADENA(char *cadena);
void ENVIAR_NUMERO(int numero,char base);

void ENVIAR_NUMERO(int numero,char base)
{char cadena[6];
  itoa(cadena,numero,base);
  ENVIAR_CADENA(cadena);  
}
void ENVIAR_CADENA(char *cadena)
{
    for (unsigned int i=0;cadena[i]!=0;i++)
    {
        ENVIAR_DATO (cadena[i]);
    }
}
void ENVIAR_DATO(char DATO)
{
//    while (TXSTAbits.TRMT==0);
    while (!TXSTAbits.TRMT);
    TXREG=DATO;
    while (!TXSTAbits.TRMT);
    
}
void INICIALIZA_COMUNICACION_SERIAL9600(void)
{
    BAUDCON=0;
    SPBRGH=0;
    SPBRGL=51;
    TXSTA=0b00100000;
    RCSTA=0b10010000;   
    TRISC|=0b11000000;
    PIE1bits.RCIE=1;
    PIR1bits.RCIF=0;
    INTCONbits.PEIE=1;
    INTCONbits.GIE=1;
    
}


#ifdef	__cplusplus
}
#endif

#endif	/* COMUNICACION_SERIAL_H */

