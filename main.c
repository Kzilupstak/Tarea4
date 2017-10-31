#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tm4c123gh6pm.h"

void ConfiguraUART0(void){
	// Habilitamos reloj para el UART0
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;
	// Habilitamos reloj para GPIOA
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	// Inhabilitamos el UART0
	UART0_CTL_R &= ~UART_CTL_UARTEN;
	// Velocidad 19200bps (Fsysclk = 16MHz)
	UART0_IBRD_R = (UART0_IBRD_R & 0xFFFF0000) | 52;
	UART0_FBRD_R = (UART0_FBRD_R & 0xFFFFFFC0) | 5;
	// 8, N, 2, FIFOs habilitados
	UART0_LCRH_R = (UART0_LCRH_R & 0xFFFFFF00) | 0x7A;
	// Habilitamos el UART0
	UART0_CTL_R |= UART_CTL_UARTEN;
	// Desactivamos modo analógico en PA0 y PA1
	GPIO_PORTA_AMSEL_R &= ~(0x03);
	// Activamos funciones digitales en PA0 y PA1
	GPIO_PORTA_DEN_R |= 0x03;
	// Activamos funciones alternas en PA0 y PA1
	GPIO_PORTA_AFSEL_R |= 0x03;
	// Conectamos UART0 a PA0 y PA1
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00)|0x00000011;
}

uint8_t Datos[11];

void TransmitCharacter (uint8_t Character){
	while ((UART0_FR_R & UART_FR_TXFF) !=0);	//Esperar a que el transmisor este vacío para poder dar otro dato
	UART0_DR_R = Character;
}

uint8_t ReceiveCharacter (void){
	uint8_t temp;
	while ((UART0_FR_R & UART_FR_RXFE) !=0);	//Esperar a que al receptor le llegue un dato y se cargue
	temp = UART0_DR_R;
	return temp;
}

void TransmitMessage (uint8_t Message[]){
	uint8_t caracter;
	uint8_t i=0;
	caracter = Message[i++];
	while (caracter != '\0'){
		TransmitCharacter(caracter);
		caracter = Message[i++];
	}
}

unsigned char Echo(void){		//FUNCION DE ECO QUE REGRESA EL VALOR ESCRITO EN EL HYPERTERMINAL PARA VISUALIZARLO
	unsigned char datoRx;
	while(1){
			// Esperamos hasta que el Buffer Rx se llene del dato
			while(UART0_FR_R & UART_FR_RXFE);
			// Se lee el dato recibido
			datoRx = UART0_DR_R;
			// Esperamos hasta que el Buffer Tx se vacie del dato
			while(UART0_FR_R & UART_FR_TXFF);
			// Se escribe el dato para su transmisión
			UART0_DR_R = datoRx;
			return datoRx;
	} //Fin de while
}

float operacion(char arreglo1[], char arreglo2[], char signo){

	float numero1, numero2, resultado;

	numero1 = atof(arreglo1);
	numero2 = atof(arreglo2);

	switch (signo){
 	 	 case 42:			//multiplicacion
 	 	 	 resultado = numero1 * numero2;
 	 	 	 break;
 	 	 case 43:			//suma
 	 	 	 resultado = numero1 + numero2;
 	 	 	 break;
 	 	 case 45:			//resta
 	 	 	 resultado = numero1 - numero2;
 	 	 	 break;
 	 	 case 47:			//division
 	 	 	 resultado = numero1 / numero2;
 	 	 	 break;
 	 }
 	 return resultado;
}

void ftoa(float valor){

	uint32_t m=0, GuardarDato = 0;
	//Esta función convierte un número a caracter ASCII y lo guarda en un arreglo

	//Convertimos el float a uint32_t
		uint32_t n;
		n = valor * 1000;

	GuardarDato = n/1000000000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Millones
		m++;
	}
		n = n%1000000000; 							// n ahora esta entre 0.000 y 999999.999

	GuardarDato = n/100000000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Centenas de miles
		m++;
	}
		n = n%100000000; 							// n ahora esta entre 0.000 y 99999.999

	GuardarDato = n/10000000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Decenas de miles
		m++;
	}
		n = n%10000000; 							// n ahora esta entre 0.000 y 9999.999

	GuardarDato = n/1000000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Miles
		m++;
	}
		n = n%1000000; 								// n ahora esta entre 0.000 y 999.999

	GuardarDato = n/100000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Centenas
		m++;
	}
		n = n%100000; 								// n ahora esta entre 0.000 y 99.999

	GuardarDato = n/10000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Decenas
		m++;
	}
		n = n%10000; 								// n ahora esta entre 0.000 y 9.999

	GuardarDato = n/1000;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Unidades
		m++;
	}
		n = n%1000;									// n ahora esta entre 0.000 y 0.999



	GuardarDato = n/100;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Decimas
		m++;
	}
		n = n%100;									// n ahora esta entre 0.000 y 0.099

		Datos[m] = 46;								// Punto Decimal
			m++;

	GuardarDato = n/10;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48; 				// Centesimas
		m++;
	}
		n = n%10; 									// n ahora esta entre 0.000 y 0.009

	GuardarDato = n;
	if (GuardarDato != 0){
		Datos[m] = GuardarDato + 48;				// Milesima
	}
}


int main(void){

	/* Mensajes */
	uint8_t MENSAJEINICIO[] = "BIENVENIDO AL ENTORNO PARA REALIZAR OPERACIONES ARITMETICAS\n\r";
	uint8_t REQUESTOPERATION[] = "Escriba la operacion que desea calcular: ";
	uint8_t JUMPLINE[] = "\n\r";
	uint8_t RETURNRESULT[] = "El resultado es ";
	/*Fin de los Mensajes Repetitivas*/


	unsigned long temp;
	char OPERARIT[30];
	char NUMERO1[11], NUMERO2[11];
	char CharacterToBeStored=0;
	char signo=0;
	float resultado;

	ConfiguraUART0();

	TransmitMessage(MENSAJEINICIO);
	TransmitMessage(JUMPLINE);

	while(1){
	uint8_t i=0, j=0, k=0, l=0;
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	TransmitMessage(REQUESTOPERATION);

	//Recibir Numero 1:
	do{
		if (l > 9){
			CharacterToBeStored = 32;
			OPERARIT[i] = CharacterToBeStored;
			NUMERO1[l] = CharacterToBeStored;
			TransmitCharacter(CharacterToBeStored);
			i++;
			l++;
		}

		else{
			if (CharacterToBeStored == 8){
				i--;
				l--;
			}
			else{
				CharacterToBeStored = Echo();
				OPERARIT[i] = CharacterToBeStored;
				NUMERO1[l] = CharacterToBeStored;
				i++;
				l++;
			}
		}
	}while (CharacterToBeStored != 32 );

	//Recibir Signo Operacional:
	do{
		if (j > 0){
			CharacterToBeStored = 32;
			OPERARIT[i] = CharacterToBeStored;
			TransmitCharacter(CharacterToBeStored);
		}

		else{
			if (CharacterToBeStored == 8){
				j--;
				i--;
			}
			else{
				CharacterToBeStored = Echo();
				OPERARIT[i] = CharacterToBeStored;
				signo = CharacterToBeStored;
				j++;
				i++;
			}
		}
	}while (CharacterToBeStored != 32 );

	//Recibir Numero 2:
	do{
		if (k > 9){
			CharacterToBeStored = 13;
			OPERARIT[i] = CharacterToBeStored;
			NUMERO2[k] = CharacterToBeStored;
			TransmitCharacter(CharacterToBeStored);
		}

		else{
			if (CharacterToBeStored == 8){
				k--;
				i--;

			}
			else{
				CharacterToBeStored = Echo();
				OPERARIT[i] = CharacterToBeStored;
				NUMERO2[k] = CharacterToBeStored;
				k++;
				i++;
			}
		}
	}while (CharacterToBeStored != 13 );

	TransmitMessage(JUMPLINE);
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	TransmitMessage(JUMPLINE);
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos

	TransmitMessage(RETURNRESULT);
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos

	//El numero que debe ser almacenado
	resultado = operacion(NUMERO1, NUMERO2, signo);
	ftoa(resultado);
	TransmitMessage(Datos);

	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	TransmitMessage(JUMPLINE);
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	temp = SYSCTL_RCGC2_R; 		//esperamos unos ciclos
	TransmitMessage(JUMPLINE);

	//LIMPIEZA DE LOS ARREGLOS
	memset(OPERARIT,'0',30);
	memset(NUMERO1,'0',10);
	memset(NUMERO2,'0',10);
	memset(Datos,'0',11);
	}
} // Fin de main

//breakpoint

