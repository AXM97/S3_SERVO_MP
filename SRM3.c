/*
 * File:   SRM3.c
 * Author: AXM
 * Sistema articulacion 2DOF con motor a pasos, servomotor y comunicacion serial
 * Created on nov 14, 2020, 10:33 AM
 */

// <editor-fold defaultstate="collapsed" desc="LIBRERIAS">
#include <xc.h>
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PALABRA DE CONFIGURACION">
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DEFINES RETARDOS">
#define _XTAL_FREQ 32000000     //32MHz
#define retardo_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))
#define retardo_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="DEFINES DE PINES">
/*#DEFINE ALIAS REFERENCIA EJEMPLO: #DEFINE LED RA0*/

#define IN1 LATAbits.LATA0
#define IN2 LATAbits.LATA1
#define IN3 LATAbits.LATA2
#define IN4 LATAbits.LATA3

#define SW1 PORTBbits.RB0
#define SW2 PORTBbits.RB1
#define SW3 PORTBbits.RB2
#define SW4 PORTBbits.RB3

#define servo1 LATC0

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="LIBRERIAS PROPIAS">
#include "COMUNICACION_SERIAL.h"
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PROTOTIPADO DE VARIABLES">

unsigned char i=0;      //variable para ciclos for de secuencias automaticas
char pos;               //variable enmascarada para detectar posicion de joystick
float pulso_servo = 0;  //variable con punto decimal para pulsos servo
unsigned char dato;     //variable recibe dato escrito por puerto serie usb
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PROTOTIPADO DE FUNCIONES">
void sistema_ini(void); //funcion para inicializar registros base e interrupciones
void MP_CW(void);       //funcion corre motor a pasos sentido horario
void MP_ACW(void);      //funcion corre motor a pasos sentido amti horario
void MP_STOP(void);     //funcion de paro para motor a pasos
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="PROGRAMA PRINCIPAL">

void main(void) {
    sistema_ini();
    INICIALIZA_COMUNICACION_SERIAL9600();
    retardo_ms(500);
    ENVIAR_CADENA(" SISTEMA 3 SM Y MP ");
    ENVIAR_DATO(0x0D);
    ENVIAR_CADENA(" INTRODUCE SECUENCIA 1 O 2: ");
    ENVIAR_DATO(0x0D);

    // <editor-fold defaultstate="collapsed" desc="CICLO INFINITO">
    while (1) {
        
        
        switch (dato) {     //se evalua dato recibido por serial
            case '1':
                ENVIAR_CADENA("SECUENCIA 1 ACTIVADA");
                ENVIAR_DATO(0x0d);
                for (i = 0; i < 20; i++) {MP_CW();}
                for (i = 0; i < 20; i++) { MP_ACW();}
                for (i = 0; i < 20; i++) {
                pulso_servo = 0.0001;
                retardo_ms(50);
                }
               for (i = 0; i < 20; i++) {
                pulso_servo = 0.0012;
                retardo_ms(50);
               }
                dato=0;
                break;

            case '2':
                 ENVIAR_CADENA("SECUENCIA 2 ACTIVADA");
                ENVIAR_DATO(0x0d);
                 for (i = 0; i < 20; i++) {MP_ACW();}
    
                for (i = 0; i < 20; i++) { MP_CW();}
                for (i = 0; i < 20; i++) {
                pulso_servo = 0.0008;
                retardo_ms(50);
                }
                
                for (i = 0; i < 20; i++) {
                pulso_servo = 0.0018;
                retardo_ms(50);
                }
                dato=0;
                break;

            default:                        //si no es 1 ni 2 entonces se mueve en funcion de la palanca joystick
               pos = PORTB & 0b00001111;        //enmascaramiento
        switch (pos) {                      //se evalua bits de posición de joystick
            case 0b00001110:        //derecha
                MP_ACW();
                pulso_servo = 0;

                break;
            case 0b00001101:        //abajo
                servo1 = 0;
                pulso_servo = 0;

                break;
            case 0b00001011:        //arriba

                pulso_servo = 0.0021;

                break;
            case 0b00000111:        //izquierda
                MP_CW();
                pulso_servo = 0;

                break;
            default:                //cualquier otra posicion 
                MP_STOP();
                pulso_servo = 0;
                
                break;

        }
                break;

        }
    }
    // </editor-fold>
    return;
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="INTERRUPCIÓN">
//ESTATICA EN MEMORIA INTERRUPT SERVICE RUTINE

void __interrupt() inter_servo(void) {
    if (PIR1bits.TMR1IF) {      //operaciones con timer1 para establecer pulsos de servo
        PIR1bits.TMR1IF = 0;
        servo1 = 1 - servo1;
        if (servo1) {
            TMR1 = 65536 - ((pulso_servo * _XTAL_FREQ) / 16);
        }
        else {
            TMR1 = 65536 - ((((0.02) - pulso_servo) * _XTAL_FREQ) / 16);
        }
        
    }

    if (PIR1bits.RCIF) {        //se activa bandera interrupcion por recepcion de datos serial
        PIR1bits.RCIF = 0; //REESTABLECEMOS BANDERA INTERRUPCION POR RECEPCION SERIAL
        dato = RCREG; //CARGAMOS EL CONTENIDO DE REGISTRO RECEPTOR A DATO
        
        
    }
}

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="FUNCIONES">

void sistema_ini(void) {
    OSCCON = 0b01110000; /*COMBINACION PARA 8 MHZ O 32 MHZ DE FRECUENCIA*/
    APFCON = 0; /*ALTERNATE PIN FUNCTION CONTROL REGISTER LO COLOCAMOS EN 0 PARA DESHABILITAR LAS FUNCIONES ALTERNATIVAS DE CADA PIN*/
    ANSELA = 0; //apaga canales analogicos A
    ANSELB = 0; //apaga canales analogicos B
    LATA = 0;
    LATB = 0;
    LATC = 0;   //puertos escritos en 0
    TRISA = 0;  //puerto A solo salidas
    TRISB = 0B00001111; //puerto b entradas primeros 4 bits los demas salidas
    TRISC = 0;  //puerto C solo salidas
    WPUB = 0B00001111;  //activamos weak pull-ups para conectar los switch/joystick
    OPTION_REGbits.nWPUEN = 0;  //habilitacion de pull-ups
    //Configuracion interrupcion
    PIE1bits.TMR1IE = 1; //Habilita la interrupcion por timer
    PIR1bits.TMR1IF = 0; //Limpia la bandera del timer 1
    INTCONbits.GIE = 1; // Interrupcion Global
    INTCONbits.PEIE = 1; // Interrupcion por periferico
    //Configuracion timer
    T1CONbits.T1CKPS = 0b10; // Divisor 4
    T1CONbits.TMR1CS = 0b00; // Fosc /4
    T1CONbits.T1OSCEN = 0; // Oscilador LP deshabilitao
    T1CONbits.nT1SYNC = 1; // No sync
    T1CONbits.TMR1ON = 1; // Encender el timer

}
void MP_CW(void) {  


    IN1 = 0;
    IN2 = 0;
    IN3 = 0;
    IN4 = 1;
    retardo_ms(25);
    IN1 = 0;
    IN2 = 0;
    IN3 = 1;
    IN4 = 0;
    retardo_ms(25);
    IN1 = 0;
    IN2 = 1;
    IN3 = 0;
    IN4 = 0;
    retardo_ms(25);
    IN1 = 1;
    IN2 = 0;
    IN3 = 0;
    IN4 = 0;
    retardo_ms(25);


}

void MP_ACW(void) {

    IN1 = 1;
    IN2 = 0;
    IN3 = 0;
    IN4 = 0;
    retardo_ms(25);
    IN1 = 0;
    IN2 = 1;
    IN3 = 0;
    IN4 = 0;
    retardo_ms(25);
    IN1 = 0;
    IN2 = 0;
    IN3 = 1;
    IN4 = 0;
    retardo_ms(25);
    IN1 = 0;
    IN2 = 0;
    IN3 = 0;
    IN4 = 1;
    retardo_ms(25);

}

void MP_STOP(void) {

    IN1 = 0;
    IN2 = 0;
    IN3 = 0;
    IN4 = 0;


}
// </editor-fold>





