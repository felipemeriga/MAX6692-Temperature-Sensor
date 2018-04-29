#include "MAX6692.h"
#include "I2C.h"

int main(void) {
	uint8_t escrito1 = 1, configura = 0, escrito2 = 1, wait = 0, vai = 0;

	uint32_t cont1 = 2000000, cont2 = 2000000;

	uint8_t dado1 = 0, dado2 = 0;

	uint16_t dado = 0;

	float temp;

	TypeMAX6692Event evento1 = GenStart, evento2 = Stopped, evento3 = Stopped;

	I2C1_RCC_Conf();
	I2C1_GPIO_Conf();
	I2C1_Conf();

	while (1) {
		if (configura == 0) { // Escreve um único byte na memória
			if (MAX6692_WR(0x00, 0x09, MAX6692_SlaveAddr, &evento1)
					== Stopped) {
				configura = 1;
				wait = 1;
			}
		} else {
			if (wait == 1) { // Aguarda o byte ser escrito
				if (vai == 0) {
					if (--cont1 == 0) {
						wait = 0;
						escrito1 = 0;
						evento2 = GenStart;
					}
				} else if (vai == 1) {
					if (--cont2 == 0) {
						wait = 0;
						escrito2 = 0;
						evento3 = GenStart;
					}
				}
			} else {
				if (escrito1 == 0) { // Lê da memória o byte escrito de forma ininterrupta
									 // espera um tempo (cont==50000) para lê-lo novamente
					if (MAX6692_RD(&dado1, 0x01, MAX6692_SlaveAddr, &evento2)
							== Stopped) {
						escrito1 = 1;
						cont1 = 50000;
						wait = 1;
						vai = 1;
					}
				} else if (escrito2 == 0) {
					if (MAX6692_RD(&dado2, 0x10, MAX6692_SlaveAddr, &evento3)
							== Stopped) {
						escrito2 = 1;
						cont2 = 50000;
						wait = 1;
						vai = 0;

						dado = (uint16_t)(dado1 << 3) + (uint16_t)(dado2 >> 5);

						temp = ((float)dado)/8;

					}
				}
			}
		}
	}
}
