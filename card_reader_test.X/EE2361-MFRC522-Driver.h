
#ifndef EE2361_MFRC522_H
#define	EE2361_MFRC522_H

#include "xc.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "EE2361-Delay.h"
#include "EE2361-MFRC522-Constants.h"

// A struct used for passing the UID of a PICC.

typedef struct {
    uint8_t size; // Number of bytes in the UID. 4, 7 or 10.
    uint8_t uidByte[10];
    uint8_t sak; // The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} Uid;

// Member variables
extern Uid uid; // Used by PICC_ReadCardSerial().

bool MFRC_init();
void MFRC_reset();
uint8_t MFRC_getVersion();
bool MFRC_readCardSerial();
bool MFRC_isNewCardPresent();
void MFRC_printUID(char *output, Uid *uid);


#endif