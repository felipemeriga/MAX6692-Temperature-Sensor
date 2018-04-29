/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

/* includes */

#include "stm32f10x.h"


/* defines */

#define I2C1_SCL				GPIO_Pin_6
#define I2C1_SDA				GPIO_Pin_7
#define I2C1_GPIO				GPIOB
#define I2C_SLAVE_ADDRESS7		0xA0
#define I2C_Speed				400000

/* Prototypes */

 void I2C1_GPIO_Conf(void);
 void I2C1_RCC_Conf(void);
 void I2C1_Conf(void);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H */
