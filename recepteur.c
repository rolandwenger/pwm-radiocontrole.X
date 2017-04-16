#include <xc.h>
#include "pwm.h"
#include "test.h"

#define CAPTURE_FLANC_MONTANT 0b101
#define CAPTURE_FLANC_DESCENDANT 0b100

/**
 * Initialise le hardware pour l'émetteur.
 */
static void recepteurInitialiseHardware() {
    
    TRISBbits.RB0 = 1; //configure l'entrée RB0 comme entrée
    TRISAbits.RA4 = 1; //configure l'entrée RA4 comme entrée
    
     // Activer le PWM sur CCP4
    CCP4CONbits.CCP4M = 0xC; // Active le CCP4.
	CCPTMRS1bits.C4TSEL = 0; // CCP4 branché sur tmr2
    
    // Activer le PWM sur CCP5
    CCP5CONbits.CCP5M = 0xC; // Active le CCP5.
	CCPTMRS1bits.C5TSEL = 0; // CCP5 branché sur tmr2
    
    //config RB4
    TRISBbits.RB4 = 1; //configure l'entrée RB4 comme entrée
    
    
    // Commun
    //Config Timer2
/*	T2CONbits.TMR2ON = 1; // Active le tmr2
    T2CONbits.T2OUTPS = 0;   // pas de division en sortie pour trm2
    T2CONbits.T2CKPS = 1;   // dision du prescaler par 4, 0-125 en 2 ms
	PR2 = 125; // interruption lorsque le timer dépasse 125
    
    // Activer les interruptions du temporisateur 2:
    PIE1bits.TMR2IE = 1;      // Active les interruptions du TMR2.
    IPR1bits.TMR2IP = 1;     // Interruptions de haute priorité
    
      // Activer les interruptions:
    RCONbits.IPEN = 1;          // Active les niveaux d'interruptions.
    INTCONbits.GIEH = 1;        // Active les interruptions de haute priorité.
    INTCONbits.GIEL = 1;        // Active les interruptions de basse priorité.
    
     // Activer le PWM sur CCP1
    CCP1CONbits.CCP1M = 0xC; // Active le CCP1.
	CCPTMRS0bits.C1TSEL = 0; // CCP1 branché sur tmr2
	TRISCbits.RC2 = 0; // Active la sortie du CCP1.
    
    // Activer le PWM sur CCP3
    CCP3CONbits.CCP3M = 0xC; // Active le CCP3.
	CCPTMRS0bits.C3TSEL = 0; // CCP3 branché sur tmr2
	TRISCbits.RC6 = 0; // Active la sortie du CCP3.
    
    
    //Config Timer2
 /*	T2CONbits.TMR2ON = 1; // Active le tmr2
    T2CONbits.T2OUTPS = 0;   // pas de division en sortie pour trm2
    T2CONbits.T2CKPS = 1;   // dision du prescaler par 4, 0-125 en 2 ms
	PR2 = 125; // interruption lorsque le timer dépasse 200
    
    // Activer les interruptions du temporisateur 2:
    PIE1bits.TMR2IE = 1;      // Active les interruptions du TMR2.
    IPR1bits.TMR2IP = 1;     // Interruptions de haute priorité
    
    // Activer le PWM sur CCP1
    CCP1CONbits.CCP1M = 0xC; // Active le CCP1.
	CCPTMRS0bits.C1TSEL = 0; // CCP1 branché sur tmr2
	TRISCbits.RC2 = 0; // Active la sortie du CCP1.
    
    // Activer le PWM sur CCP3
    CCP3CONbits.CCP3M = 0xC; // Active le CCP3.
	CCPTMRS0bits.C3TSEL = 0; // CCP3 branché sur tmr2
	TRISCbits.RC6 = 0; // Active la sortie du CCP3.
    //PWM3CONbits.P3DC = 
    
    //config RB4
    TRISBbits.RB4 = 1; //configure l'entrée RB4 comme entrée
    
    // Activer les interruptions:
    RCONbits.IPEN = 1;          // Active les niveaux d'interruptions.
    INTCONbits.GIEH = 1;        // Active les interruptions de haute priorité.
    INTCONbits.GIEL = 1;        // Active les interruptions de basse priorité.
    
    
    //config des ports
    ANSELA=0;   //configure le port A comme digital
    ANSELB=0;   //configure le port B comme digital
    TRISAbits.RA4 = 1; //configure l'entrée RA4 comme entrée
    TRISBbits.RB0 = 1; //configure l'entrée RB0 comme entrée
    
    // Activer le PWM sur CCP4
    CCP4CONbits.CCP4M = 0xC; // Active le CCP4.
	CCPTMRS1bits.C4TSEL = 0; // CCP4 branché sur tmr2
    
    // Activer le PWM sur CCP5
    CCP5CONbits.CCP5M = 0xC; // Active le CCP5.
	CCPTMRS1bits.C5TSEL = 0; // CCP5 branché sur tmr2
 */ 
}

/**
 * Point d'entrée des interruptions basse priorité.
 */
void recepteurInterruptions() {
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

    if (PIR4bits.CCP4IF) {
        if (PORTBbits.RB0) {
            pwmDemarreCapture(1, CCPR4);
            CCP4CONbits.CCP4M = CAPTURE_FLANC_DESCENDANT;
        } else {
            pwmCompleteCapture(1, CCPR4);            
            CCP4CONbits.CCP4M = CAPTURE_FLANC_MONTANT;
        }
        PIR4bits.CCP4IF = 0;
    }

    if (PIR4bits.CCP5IF) {
        if (PORTAbits.RA4) {
            pwmDemarreCapture(0, CCPR5);
            CCP5CONbits.CCP5M = CAPTURE_FLANC_DESCENDANT;
        } else {
            pwmCompleteCapture(0, CCPR5);            
            CCP5CONbits.CCP5M = CAPTURE_FLANC_MONTANT;
        }
        PIR4bits.CCP5IF = 0;        
    }
}

/**
 * Point d'entrée pour l'émetteur de radio contrôle.
 */
void recepteurMain(void) {
    recepteurInitialiseHardware();
    pwmReinitialise();

    while(1);
}
