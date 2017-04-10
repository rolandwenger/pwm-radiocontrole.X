#include "test.h"
#include "pwm.h"

#define PWM_NOMBRE_DE_CANAUX 2
#define PWM_ESPACEMENT 6

static unsigned char valeurCanal[PWM_NOMBRE_DE_CANAUX];

/**
 * Convertit une valeur signée générique vers une valeur directement
 * utilisable pour la génération PWM.
 * @param valeur Une valeur entre 0 et 255.
 * @return Une valeur entre 62 et 125.
 */
unsigned char pwmConversion(unsigned char valeurGenerique) {
    unsigned char val1;
    if (valeurGenerique > 0){
        val1 = (valeurGenerique/4);
        val1 = val1 +62;
        return val1;
    }
    return 62;
}

static unsigned char canalPret = 0;

/**
 * Indique sur quel canal la valeur doit changer.
 * @param canal Le numéro de canal.
 */
void pwmPrepareValeur(unsigned char canal) {
    if (canal == 0){
     canalPret = 0;   
    //valeurCanal [0] = canalPret;
    } else{
     canalPret = 1;
    //valeurCanal [1] = canalPret;
    }
}

/**
 * Établit la valeur du canal spécifié par {@link #pwmPrepareValeur}.
 * @param valeur La valeur du canal.
 */
void pwmEtablitValeur(unsigned char valeur) {
    if (canalPret == 0){
        valeurCanal [0]= pwmConversion(valeur);
    } else{
        valeurCanal [1]= pwmConversion(valeur);
    }
    //canalPret =  pwmConversion(valeur);
}

/**
 * Rend la valeur PWM correspondante au canal.
 * @param canal Le cana.
 * @return La valeur PWM correspondante au canal.
 */
unsigned char pwmValeur(unsigned char canal) {
    if (canal == 0){
        return valeurCanal[0];
    } else {
      return valeurCanal[1];
    } 
}

static unsigned char espacement = 0;

/**
 * Indique si il est temps d'émettre une pulsation PWM.
 * Sert à espacer les pulsation PWM pour les rendre compatibles
 * avec la norme de radio contrôle.
 * @return 255 si il est temps d'émettre une pulse. 0 autrement.
 */
unsigned char pwmEspacement() {
   unsigned char val;
   switch(espacement){
        case 0:
            val=255;
            break;
       default:
            val=0;
    }
       espacement++;
        if (espacement > 6){
        espacement = 0;
         }
       return val;
}

unsigned int capture[PWM_NOMBRE_DE_CANAUX];

/**
 * Démarre une capture sur le canal indiqué.
 * @param canal Numéro du canal.
 * @param instant Instant de démarrage de la capture.
 */
void pwmDemarreCapture(unsigned char canal, unsigned int instant) {
   //unsigned char valeur;
   //  valeur = pwmValeur(canal);
    if (canal == 0){
        capture[0] = instant;
    } else {
        capture[1] = instant;
    }
}

/**
 * Complète une capture PWM, et met à jour le canal indiqué.
 * @param canal Le numéro de canal.
 * @param instant L'instant de finalisation de la capture.
 */
void pwmCompleteCapture(unsigned char canal, unsigned int instant) {
    if (canal == 0){
        if (instant >= capture[0]){
           capture[0] = (instant - capture[0]); 
        } else {
            capture[0] = (65536 - capture[0]);
            capture[0] = (instant + capture[0]); 
        }       
        valeurCanal[0] = capture[0];
    } else {
        if (instant >= capture[1]){
           capture[1] = (instant - capture[1]); 
        } else {
            capture[1] = (65536 - capture[1]);
            capture[1] = (instant + capture[1]); 
        } 
        valeurCanal[1] = capture[1];
    }
}

/**
 * Réinitialise le système PWM.
 */
void pwmReinitialise() {
    valeurCanal[0]=0;
    valeurCanal[1]=0;
    capture[0] = 0;
    capture[1] = 0;
   // canalPret = 0;
   //espacement=0;
}

#ifdef TEST
void testConversionPwm() {
    testeEgaliteEntiers("PWMC001", pwmConversion(  0),  62);
    testeEgaliteEntiers("PWMC002", pwmConversion(  4),  63);

    testeEgaliteEntiers("PWMC003", pwmConversion(126),  93);
    
    testeEgaliteEntiers("PWMC004", pwmConversion(127),  93);
    testeEgaliteEntiers("PWMC005", pwmConversion(128),  94);
    testeEgaliteEntiers("PWMC006", pwmConversion(129),  94);
    
    testeEgaliteEntiers("PWMC007", pwmConversion(132),  95);

    testeEgaliteEntiers("PWMC008", pwmConversion(251), 124);
    testeEgaliteEntiers("PWMC009", pwmConversion(255), 125);
}
void testEtablitEtLitValeurPwm() {
    pwmReinitialise();
    
    pwmPrepareValeur(0);
    pwmEtablitValeur(80);
    testeEgaliteEntiers("PWMV01", pwmValeur(0), pwmConversion(80));
    testeEgaliteEntiers("PWMV02", pwmValeur(1), 0);

    pwmPrepareValeur(1);
    pwmEtablitValeur(180);
    testeEgaliteEntiers("PWMV03", pwmValeur(0), pwmConversion( 80));
    testeEgaliteEntiers("PWMV04", pwmValeur(1), pwmConversion(180));
}
void testEspacementPwm() {
    unsigned char n;
    
    pwmReinitialise();

    for (n = 0; n < PWM_ESPACEMENT; n++) {
        testeEgaliteEntiers("PWME00", pwmEspacement(), 0);
    }

    testeEgaliteEntiers("PWME01", pwmEspacement(), 255);
    
    for (n = 0; n < PWM_ESPACEMENT; n++) {
        testeEgaliteEntiers("PWME00", pwmEspacement(), 0);
    }

    testeEgaliteEntiers("PWME01", pwmEspacement(), 255);    
}
void testCapturePwm() {
    
    pwmDemarreCapture(0, 0);
    pwmCompleteCapture(0, 80);
    
    pwmDemarreCapture(1, 10000);
    pwmCompleteCapture(1, 10090);

    testeEgaliteEntiers("PWMC01a", pwmValeur(0), 80);
    testeEgaliteEntiers("PWMC01b", pwmValeur(1), 90);
    
    pwmDemarreCapture(0, 65526);
    pwmCompleteCapture(0, 80);
    
    pwmDemarreCapture(1, 65516);
    pwmCompleteCapture(1, 80);
    
    testeEgaliteEntiers("PWMC02a", pwmValeur(0), 90);
    testeEgaliteEntiers("PWMC02b", pwmValeur(1), 100);    
}
void testPwm() {    
    testConversionPwm();
    testEtablitEtLitValeurPwm();
    testEspacementPwm();
    testCapturePwm();
}

#endif
