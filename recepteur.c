#include <xc.h>
#include "pwm.h"
#include "test.h"

#define CAPTURE_FLANC_MONTANT 0b101
#define CAPTURE_FLANC_DESCENDANT 0b100

/**
 * Initialise le hardware pour l'émetteur.
 */
static void recepteurInitialiseHardware() {
    // À faire.
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
