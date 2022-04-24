/*
 * File:   delay_lab08.c
 * Author: aleja
 *
 * Created on April 18, 2022, 1:30 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 4000000
#define _tmr0_value 700

/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
uint8_t bandera =  0b001;

uint8_t contador;    
uint8_t remainder;     
uint8_t unidades;       //variable de unidades 
uint8_t decenas;        //variable de decenas 
uint8_t centenas;       //variable de centenas

uint8_t display_1;
uint8_t display_2;
uint8_t display_3;

/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void setup(void);

/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    if(PIR1bits.ADIF){             
        if(ADCON0bits.CHS == 0)
        {    // Verificamos sea AN0 el canal seleccionado
            PORTC = ADRESH;         // Mostramos ADRESH en PORT
        }
        
        else if (ADCON0bits.CHS == 1)
        {
            contador = ADRESH; 
            
        }
    
        PIR1bits.ADIF = 0;          // Limpiamos bandera de interrupción
    }
    
    if (T0IF)
    {
                if (bandera ==  0)
                {
                    PORTD = display_3;
                    PORTE = 0b001;
                    bandera = 1;
                    
                }
                
                else if (bandera == 1)
                {
                    PORTD = display_2;
                    PORTE = 0b010; 
                    bandera = 2;
                 
                }
                
                else if (bandera ==  2)
                {
                    PORTD = display_1;
                    PORTE = 0b100 ;  
                    bandera = 0; 
                }
           
             INTCONbits.T0IF = 0; 
     }
    
    return;
}

/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    int tabla[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};
    
    while(1)
    {
        if(ADCON0bits.GO == 0)
        {  
            if (ADCON0bits.CHS == 0){
                ADCON0bits.CHS = 1;
            }
            else if(ADCON0bits.CHS == 1){
                ADCON0bits.CHS = 0;
            } 
        __delay_us(40);
        ADCON0bits.GO = 1;              // Iniciamos proceso de conversión 
        }
        
        centenas = contador/100 ;
        remainder = contador % 100;
        decenas = remainder /10;
        unidades = remainder % 10;
        
        display_1 = tabla[unidades];
        display_2 = tabla[decenas];
        display_3 = tabla[centenas];
     
    
    }
    return;
}

/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){
    ANSEL = 0b00000011; // AN0 como entrada analógica
    ANSELH = 0;         // I/O digitales)
    
    
    TRISA = 0b00000011; // AN0 y AN1 como entrada
    PORTA = 0; 
    
    TRISC = 0;          //salida contador 1
    PORTC = 0;
    
    TRISD = 0;          //salida contador 2
    PORTD = 0;         
    
    TRISE = 0;          //banderas display 
    PORTE = 0;  
    
    
    // Configuración reloj interno
    OSCCONbits.IRCF = 0b0110;   // 4MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    // Configuración ADC
    ADCON0bits.ADCS = 0b01;     // Fosc/8
    ADCON1bits.VCFG0 = 0;       // VDD
    ADCON1bits.VCFG1 = 0;       // VSS
    ADCON0bits.CHS = 0b0000;    // Seleccionamos el AN0
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
    ADCON0bits.ADON = 1;        // Habilitamos modulo ADC
    __delay_us(40);             // Sample time
    
    // TMR0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA  = 0;
    OPTION_REGbits.PS   = 0b111; 
    
    // Configuracion interrupciones
    PIR1bits.ADIF = 0;          // Limpiamos bandera de ADC
    PIE1bits.ADIE = 1;          // Habilitamos interrupcion de ADC
    INTCONbits.PEIE = 1;        // Habilitamos int. de perifericos
    INTCONbits.GIE = 1;         // Habilitamos int. globales
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    
}

