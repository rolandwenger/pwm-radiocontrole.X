#include <xc.h>
#include "pwm.h"

/**
 * Initialise le hardware pour l'émetteur.
 */
static void emetteurInitialiseHardware() {
    // À faire.
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
