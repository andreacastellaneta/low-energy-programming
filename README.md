# low-energy-programming
STM32 Low Energy Programming for IoT - TP 5AE ESPE

Réalisé par CASTELLANETA Andrea et CROUARD Christale

On utilisera la bibliothèque LL de préférence (plutôt que HAL).

En vue d'avoir au final toutes les expériences dans un seul programme, deux variables globales seront utilisées :

- la variable expe portant le numéro de l'expérience, qui sera préservée dans le backup register 0 du module RTC

Cette variable sera incrémentée au reset, si le bouton bleu (PC13) est pressé à cet instant

- la variable blue_mode qui sera mise à zéro au reset et mise à 1 lors d'une action sur le bouton bleu (PC13) après le reset

N.B. la variable expe sera réinitialisée à 1 lors d'une coupure prolongée d'alimentation, ou du dépassement du dernier numéro d'expérience.

Etapes de développement :

[expe = 1] clignotement de la LED au moyen du timer SysTick en interruption en remplacement des boucles de temporisation

- Période du timer : 10ms

- Période de la LED : 2s, durée d'impulsion : expe * 50 ms

[expe = 1] exploitation du mode sleep à chaque tour de la boucle principale

- blue_mode : dans la boucle principale le processeur passe en mode sleep (jusqu'à la prochaine interruption)

[expe = 2] exploitation de l'oscillateur interne MSI à 24 MHz en direct en remplacement de la configuration initiale (démo ST) qui était MSI à 4 MHz + PLL à 80 MHz 
(ne pas oublier d'ajuster le parametre flash latency avant d'augmenter la fréquence de l'oscillateur)

[expe = 2] évaluation de la stabilité en fréquence de l'oscillateur MSI

- émission d'un signal carré à 50 Hz sur PC10 par l'interruption systick pour mesure de période

- blue_mode : asservissement ("calibration") de l'oscillateur MSI sur l'oscillateur à quartz d'horlogerie LSE 32kHz pour évaluation de la dérive thermique sans et avec cet asservissement

exploitation du module RTC :

- observer la persistance de l'heure et des backup registers en cas de reset

- mettre en place la sauvegarde de expe dans le backup register 0 et son incrémentation au reset si le bouton bleu (PC13) est pressé à cet instant
programme d'expériences séquencées de mesure de consommation incluant modes sleep, voltage range, stop, standby, shutdown, reveil par RTC (wakeup timer)
