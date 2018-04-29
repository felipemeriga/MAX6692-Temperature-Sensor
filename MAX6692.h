/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAX6692_H
#define __MAX6692_H

#ifdef __cplusplus
 extern "C" {
#endif

/* includes */

#include "stm32f10x.h"


/* Typedefs */

 typedef enum {
 	GenStart = 0,
 	wait_EV5,
 	SlaveAddr_transmitter,
 	wait_EV6,
 	SendAddr,
 	wait_EV8_SendAddr,
 	SendData,
 	wait_EV8_SendData,
 	GenStop,
 	New_start,
 	wait_EV5_New_Start,
 	SlaveAddr_receiver,
 	wait_EV6_B,
 	wait_ADDR_SET,
 	ReadData,
 	wait_EV7,
 	wait_EV7_2,
 	wait_EV7_3,
 	wait_EV7_B,
 	Stopped

 } TypeMAX6692Event;

#define MAX6692_SlaveAddr	0b10011000

 /* Prototypes */

 TypeMAX6692Event MAX6692_WR(uint8_t dado, uint8_t ender, uint8_t SlaveAddr, TypeMAX6692Event* Event );
 TypeMAX6692Event MAX6692_RD (uint8_t* ret_data, uint8_t ender, uint8_t SlaveAddr, TypeMAX6692Event* Event);
 TypeMAX6692Event MAX6692_SB (uint8_t ender, uint8_t SlaveAddr, TypeMAX6692Event* Event);
 TypeMAX6692Event MAX6692_RB (uint8_t* ret_data, uint8_t SlaveAddr, TypeMAX6692Event* Event);

#ifdef __cplusplus
}
#endif

#endif /* __MAX6692_H */
