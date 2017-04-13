#include <xc.h>
#include "pwm.h"

/**
 * Initialise le hardware pour l'émetteur.
 */
static void emetteurInitialiseHardware() {
    ANSELB = 0;
    //config des ports
    TRISBbits.RB1 = 1; //configure l'entrée RB1 comme entrée
    TRISBbits.RB2 = 1; //configure l'entrée RB2 comme entrée
    INTCON2bits.RBPU=0;// active les résistances de tirage
    WPUBbits.WPUB1=1;   //active la résistance de tirage du port RB1
    WPUBbits.WPUB2=1;   //active la résistance de tirage du port RB2

    //Conversion analogique digital
    ANSELBbits.ANSB3 = 1; //configuration comme entrée analogique,
    TRISBbits.RB3=1;//configure le port RB3 comme entrée
    ADCON0bits.ADON = 1;    // active le module de conversion
    ADCON0bits.CHS = 0b1001; // configuration de la conversion analogique sur an9
    ADCON2bits.ADFM = 0;    // Les 8 bits plus signifiants sur ADRESH.
    
     // Activer les interruptions:
    RCONbits.IPEN = 1;          // Active les niveaux d'interruptions.
    INTCONbits.GIEH = 1;        // Active les interruptions de haute priorité.
    INTCONbits.GIEL = 1;        // Active les interruptions de basse priorité.
    
    //config des interruptions externes
    INTCON3bits.INT1IE = 1; //active l'interruption externe 1
    INTCON3bits.INT2IE = 1; //active l'interruption externe 2
    INTCON3bits.INT1IP = 0; //priodité basse de l'interruption externe 1 RB1
    INTCON3bits.INT2IP = 0; //priodité basse de l'interruption externe 2 RB2
    INTCON2bits.INTEDG2 = 0; // Int. de INT2 sur flanc descendant.
    INTCON2bits.INTEDG1 = 0; // Int. de INT1 sur flanc descendant
    
    // Activer les interruptions du temporisateur 2:
    PIE1bits.TMR2IE = 1;      // Active les interruptions du TMR2.
    IPR1bits.TMR2IP = 1;     // Interruptions de haute priorité
    
    //Config Timer2
	T2CONbits.TMR2ON = 1; // Active le tmr2
    T2CONbits.T2OUTPS = 0;   // pas de division en sortie pour trm2
    T2CONbits.T2CKPS = 1;   // dision du prescaler par 4, 0-125 en 2 ms
	PR2 = 201; // interruption lorsque le timer dépasse 20
    
    // Activer le PWM sur CCP1
    CCP1CONbits.CCP1M = 0xC; // Active le CCP1.
	CCPTMRS0bits.C1TSEL = 0; // CCP1 branché sur tmr2
	TRISCbits.RC2 = 0; // Active la sortie du CCP1.
    
     // Activer le PWM sur CCP3
    CCP3CONbits.CCP3M = 0xC; // Active le CCP3.
	CCPTMRS0bits.C3TSEL = 0; // CCP3 branché sur tmr2
	TRISCbits.RC6 = 0; // Active la sortie du CCP3.
    
    /*   ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
  
    //config RB4
    TRISBbits.RB4 = 1; //configure l'entrée RB4 comme entrée
     
    
    
 
    //PWM3CONbits.P3DC = 
    
    ANSELBbits.ANSB3 = 1; //configuration comme entrée analogique,
    TRISBbits.RB3=1;//configure le port RB3 comme entrée 
*/
}

/**
 * Point d'entrée des interruptions pour l'émetteur.
 */
void emetteurInterruptions() {
    unsigned char p1, p3;

    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        if (pwmEspacement()) {
            p1 = pwmValeur(0);
            p3 = pwmValeur(1);
            CCPR3L = p3;
            CCPR1L = p1;
        } else {
            CCPR3L = 0;
            CCPR1L = 0;
        }
    }
    
    if (INTCON3bits.INT1F) {
        INTCON3bits.INT1F = 0;
        pwmPrepareValeur(1);
        ADCON0bits.GO = 1;
    }
    
    if (INTCON3bits.INT2F) {
        INTCON3bits.INT2F = 0;
        pwmPrepareValeur(0);
        ADCON0bits.GO = 1;
    }
    
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        pwmEtablitValeur(ADRESH);
    }
}

/**
 * Point d'entrée pour l'émetteur de radio contrôle.
 */
void emetteurMain(void) {
    emetteurInitialiseHardware();
    pwmReinitialise();

    while(1);
}
