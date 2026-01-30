#pragma once
#include <stdint.h>

extern uint16_t SMI_CMD;
extern uint8_t  ACPI_ENABLE;
extern uint8_t  ACPI_DISABLE;
extern uint16_t PM1a_CNT;
extern uint16_t PM1b_CNT;
extern uint16_t SLP_TYPa;
extern uint16_t SLP_TYPb;
extern uint16_t SLP_EN;
extern uint16_t SCI_EN;
extern uint8_t  PM1_CNT_LEN;

uint32_t *acpiCheckRSDPtr(void);  
int acpiInit(void);               
int acpiEnable(void);            
void acpi_shutdown(void);