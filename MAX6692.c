/* includes */
#include "MAX6692.h"

/* funções */

/*
 * Função de escrita randômica NÃO BLoqueante (SB - Single Byte)
 * Escreve o byte (dado) no endereço (ender) no dispositivo cuja
 * palavra de controle de 7 bits está contida em SlaveAddr
 * A variável de estados de uma instância desta máquina deve ter seu
 * endereço passado através do ponteiro Event.
 * Esta função é não bloqueante. Retorna o estado atual de sua máquina de estados
 * retorna o estado stopped ao terminar de escrever na página da memória
 */

TypeMAX6692Event MAX6692_WR(uint8_t dado, uint8_t ender, uint8_t SlaveAddr,
		TypeMAX6692Event* Event) {

	switch (*Event) {
	case GenStart:
		// Send START condition
		I2C_GenerateSTART(I2C1, ENABLE);
		*Event = wait_EV5;
		break;

	case wait_EV5:
		// Test on EV5 and clear it
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
			*Event = wait_EV6;
			// Send EEPROM address for write
			I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);
		}
		break;

	case wait_EV6:
		// Test on EV6 and clear it
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
			*Event = wait_EV8_SendAddr;
			// Escreve o endereço de memória a ser escrito
			I2C_SendData(I2C1, ender);
		}
		break;

	case wait_EV8_SendAddr:
		// Test on EV8 and clear it
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)
				|| I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
			*Event = wait_EV8_SendData;
			// Escreve o endereço de memória a ser escrito
			I2C_SendData(I2C1, dado);
		}
		break;

	case wait_EV8_SendData:
		// Test on EV8 and clear it
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)
				|| I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
			*Event = Stopped;
			// Send STOP condition
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		break;

	default:
		break;
	}

	return *Event;

}

/*
 * Função que lê um único byte e o retorna através do ponteiro ret_data.
 * O byte é lido do endereço ender do registrador de temperatura requerido do MAX6692.
 * SlaveAddr é a palavra de controle de 7 bits deslocada um bit à esquerda.
 * Event aponta para a variável que controla o estado da máquina.
 * Esta função é não bloqueante. Retorna o estado atual de sua máquina de estados
 * Ao final, retorna o estado stopped, indicando que a leitura foi realizada
 */

TypeMAX6692Event MAX6692_RD(uint8_t* ret_data, uint8_t ender, uint8_t SlaveAddr,
		TypeMAX6692Event* Event) {

	switch (*Event) {
	case GenStart:
		// Habilita ACK
		I2C_AcknowledgeConfig(I2C1, ENABLE);
		// Send START condition
		I2C_GenerateSTART(I2C1, ENABLE);
		*Event = wait_EV5;
		break;

	case wait_EV5:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
			*Event = wait_EV6;
			// Send EEPROM address for write
			I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);
		}
		break;

	case wait_EV6:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
			*Event = wait_EV8_SendAddr;
			// Escreve o endereço de memória a ser lido
			I2C_SendData(I2C1, ender);
		}
		break;

	case wait_EV8_SendAddr:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)
				|| I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
			*Event = wait_EV5_New_Start;
			/* Send START condition a second time */
			I2C_GenerateSTART(I2C1, ENABLE);
		}
		break;

	case wait_EV5_New_Start:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
			*Event = wait_ADDR_SET;
			/* Send EEPROM address for read */
			I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Receiver);
		}
		break;

	case wait_ADDR_SET:
		// Verifica se ADDR=1 mas não o reseta
		if (I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002) {
			*Event = wait_EV6_B;
			/* Disable Acknowledgement */
			I2C_AcknowledgeConfig(I2C1, DISABLE); // Bit CR1.ACK<-0 apenas após ADDR=1, mas antes de resetá-lo
		}
		break;

	case wait_EV6_B:
		// Verifica, além de outros, se ADDR==1 e o reseta
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
			*Event = wait_EV7;
			/* Send STOP Condition */
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		break;

	case wait_EV7:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
			*Event = Stopped;
			/*Get the data from memory*/
			*ret_data = I2C_ReceiveData(I2C1);
		}
		break;

	default:
		break;
	}

	return *Event;

}

/*
 * Função que envia o byte do registrador de configuração, porém sem escrever um dado.
 * Configura o registrador no endereço (ender) no dispositivo cuja
 * palavra de controle de 7 bits está contida em SlaveAddr
 * A variável de estados de uma instância desta máquina deve ter seu
 * endereço passado através do ponteiro Event.
 * Esta função é não bloqueante. Retorna o estado atual de sua máquina de estados
 * retorna o estado stopped ao terminar de escrever na página da memória
 */

TypeMAX6692Event MAX6692_SB(uint8_t ender, uint8_t SlaveAddr,
		TypeMAX6692Event* Event) {

	switch (*Event) {
		case GenStart:
			// Send START condition
			I2C_GenerateSTART(I2C1, ENABLE);
			*Event = wait_EV5;
			break;

		case wait_EV5:
			// Test on EV5 and clear it
			if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
				*Event = wait_EV6;
				// Send EEPROM address for write
				I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);
			}
			break;

		case wait_EV6:
			// Test on EV6 and clear it
			if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
				*Event = wait_EV8_SendAddr;
				// Escreve o endereço de memória a ser escrito
				I2C_SendData(I2C1, ender);
			}
			break;

		case wait_EV8_SendAddr:
			// Test on EV8 and clear it
			if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)
					|| I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
				*Event = Stopped;
				// Send STOP condition
				I2C_GenerateSTOP(I2C1, ENABLE);
			}
			break;

		default:
			break;
		}

		return *Event;

}

/*
 * Função que lê um único byte e o retorna através do ponteiro ret_data.
 * O byte é lido do endereço do registrador anteriormente configurado em uma função
 * MAX6692_RD ou MAX6692_WR.
 * SlaveAddr é a palavra de controle de 7 bits deslocada um bit à esquerda.
 * Event aponta para a variável que controla o estado da máquina.
 * Esta função é não bloqueante. Retorna o estado atual de sua máquina de estados
 * Ao final, retorna o estado stopped, indicando que a leitura foi realizada
 */

TypeMAX6692Event MAX6692_RB(uint8_t* ret_data, uint8_t SlaveAddr,
		TypeMAX6692Event* Event) {

	switch (*Event) {
	case GenStart:
		// Habilita ACK
		I2C_AcknowledgeConfig(I2C1, ENABLE);
		// Send START condition
		I2C_GenerateSTART(I2C1, ENABLE);
		*Event = wait_EV5;
		break;

	case wait_EV5:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
			*Event = wait_ADDR_SET;
			/* Send EEPROM address for read */
			I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Receiver);
		}
		break;

	case wait_ADDR_SET:
		// Verifica se ADDR=1 mas não o reseta
		if (I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002) {
			*Event = wait_EV6_B;
			/* Disable Acknowledgement */
			I2C_AcknowledgeConfig(I2C1, DISABLE); // Bit CR1.ACK<-0 apenas após ADDR=1, mas antes de resetá-lo
		}
		break;

	case wait_EV6_B:
		// Verifica, além de outros, se ADDR==1 e o reseta
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
			*Event = wait_EV7;
			/* Send STOP Condition */
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		break;

	case wait_EV7:
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
			*Event = Stopped;
			/*Get the data from memory*/
			*ret_data = I2C_ReceiveData(I2C1);
		}
		break;

	default:
		break;
	}

	return *Event;

}
