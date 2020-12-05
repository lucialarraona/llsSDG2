/*
 * piTankGo_1.h
 *
 *  Created on: 11 de enero de 2019
 *      Author: FFM
 */

#ifndef _PITANKGO_1_H_
#define _PITANKGO_1_H_

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "kbhit.h" // Para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "player.h"
#include "torreta.h"

#include <string.h>
#include <errno.h>

#include "fsm.h"
#include "tmr.h"

// Posibles estados de las FSMs
enum fsm_state {

	WAIT_START,
	WAIT_KEY,
	WAIT_PUSH,
	WAIT_NEXT,
	WAIT_MOVE,
	WAIT_END,
	JOYSTICK_UP,
	JOYSTICK_DOWN,
	JOYSTICK_LEFT,
	JOYSTICK_RIGHT,
	TRIGGER_BUTTON
};

typedef struct {
	TipoTorreta torreta; // Objeto para el control de la torreta
	TipoPlayer player; // Reproductor de efectos
	char teclaPulsada;  // Variable que almacena la ultima tecla pulsada
	int debug; // Variable que habilita o deshabilita la impresion de mensajes por salida estandar
} TipoSistema;


//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------
int ConfiguraSistema (TipoSistema *p_sistema);
int InicializaSistema (TipoSistema *p_sistema);

//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------
PI_THREAD(thread_explora_teclado_PC);

//funciones del teclado

int CompruebaColumnTimeout (fsm_t* this);
void row_1_isr (void);
void row_2_isr (void);
void row_3_isr (void);
void row_4_isr (void);

void col_1 (fsm_t* this);
void col_2 (fsm_t* this);
void col_3 (fsm_t* this);
void col_4 (fsm_t* this);

int key_pressed (fsm_t* this);
void process_key (fsm_t* this);

void timer_duracion_columna_isr (union sigval value);
#endif /* _PITANKGO_1_H_ */
