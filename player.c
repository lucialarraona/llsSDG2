
#include "player.h"
#include <string.h>
#include <softTone.h>
#include <pthread.h>

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

//------------------------------------------------------
// void InicializaEfecto (TipoEfecto *p_efecto)
//
// Descripcion: inicializa los parametros del objeto efecto.
// Ejemplo de uso:
//
// ...
//
// TipoEfecto efecto_demo;
//
// if ( InicializaEfecto (&efecto_demo, "DISPARO", frecuenciasDisparo, tiemposDisparo, 16) < 0 ) {
// 		printf("\n[ERROR!!!][InicializaEfecto]\n");
// 		fflush(stdout);
// 		}
//
// ...
//
//------------------------------------------------------
int InicializaEfecto (TipoEfecto *p_efecto, char *nombre, int *array_frecuencias, int *array_duraciones, int num_notas) {
	// A completar por el alumno...
	// ...
	int i = 0;
	strcpy(p_efecto->nombre , nombre);
	for ( i=0 ; i<num_notas; i++){
		p_efecto -> frecuencias[i]=array_frecuencias[i];
		p_efecto -> duraciones[i]=array_duraciones[i];

	}
	p_efecto->num_notas = num_notas;

	return p_efecto->num_notas;
}

// Procedimiento de inicializacion del objeto especifico
// Nota: parte inicialización común a InicializaPlayDisparo y InicializaPlayImpacto

void InicializaPlayer (TipoPlayer *p_player) {
	p_player -> posicion_nota_actual=0;
	p_player-> duracion_nota_actual=p_player->p_efecto->frecuencias[p_player->posicion_nota_actual];
	p_player-> frecuencia_nota_actual=p_player->p_efecto->duraciones[p_player->posicion_nota_actual];

	softToneWrite(PLAYER_PWM_PIN, p_player-> posicion_nota_actual);

	 p_player-> tmr_duracion_nota = tmr_new(timer_player_duracion_nota_actual_isr); // lo creas
	 tmr_startms((tmr_t*)(p_player->tmr_duracion_nota), p_player->duracion_nota_actual); // lo inicializas para que empiece a cotar en cuanto inicialices el player

	 piLock(STD_IO_BUFFER_KEY);
	 printf("[Nueva nota (%d de %d)]\n",
			 p_player-> posicion_nota_actual +1,
			 p_player-> p_efecto->num_notas);

	 printf( "[NOTA %d][FREC %d][DURA %d]\n",
	 					p_player -> posicion_nota_actual +1,
	 					p_player -> frecuencia_nota_actual,
	 					p_player -> duracion_nota_actual);
	 fflush(stdout);
	 piUnlock(STD_IO_BUFFER_KEY);


}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaStartDisparo (fsm_t* this) {
	int result = 0;

	// A completar por el alumno

	piLock(PLAYER_FLAGS_KEY);
	result = (flags_player & FLAG_START_DISPARO);
	piUnlock (PLAYER_FLAGS_KEY);

	return result;
}

int CompruebaStartImpacto (fsm_t* this) {
	int result = 0;

	// A completar por el alumno
	piLock(PLAYER_FLAGS_KEY);
	result =(flags_player & FLAG_START_IMPACTO);
	piUnlock(PLAYER_FLAGS_KEY);


	return result;
}

int CompruebaNuevaNota (fsm_t* this){
	int result = 0;

	// A completar por el alumno
	piLock(PLAYER_FLAGS_KEY);
	result=!(flags_player & FLAG_PLAYER_END );
	piUnlock(PLAYER_FLAGS_KEY);
	return result;
}

int CompruebaNotaTimeout (fsm_t* this) {
	int result = 0;

	// A completar por el alumno

	piLock(PLAYER_FLAGS_KEY);
	result =(flags_player & FLAG_NOTA_TIMEOUT);
	piUnlock(PLAYER_FLAGS_KEY);

	return result;
}

int CompruebaFinalEfecto (fsm_t* this) {
	int result = 0;

	piLock(PLAYER_FLAGS_KEY);
	result =(flags_player & FLAG_PLAYER_END);
	piUnlock(PLAYER_FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void InicializaPlayDisparo (fsm_t* this) {

	TipoPlayer *p_player = (TipoPlayer*)this->user_data;

	piLock(PLAYER_FLAGS_KEY);
	flags_player &= ~FLAG_START_DISPARO;
	piUnlock(PLAYER_FLAGS_KEY);

	p_player -> p_efecto =& (p_player -> efecto_disparo);
	InicializaPlayer(p_player);


	piLock(STD_IO_BUFFER_KEY);
	printf( "disparo on\n");
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
}

void InicializaPlayImpacto (fsm_t* this) {

	TipoPlayer *p_player = (TipoPlayer*)this->user_data;

	piLock(PLAYER_FLAGS_KEY);
	flags_player &= ~FLAG_START_IMPACTO;
	piUnlock(PLAYER_FLAGS_KEY);

	p_player ->p_efecto= &(p_player->efecto_impacto);
	InicializaPlayer(p_player);

	piLock(STD_IO_BUFFER_KEY);
	printf( "impacto on\n");
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
}

void ComienzaNuevaNota (fsm_t* this) {
	TipoPlayer *n_player = (TipoPlayer*)this->user_data;

	piLock(PLAYER_FLAGS_KEY);
	flags_player &= ~ FLAG_PLAYER_END;
	piUnlock(PLAYER_FLAGS_KEY);

	softToneWrite(PLAYER_PWM_PIN, n_player->frecuencia_nota_actual);
	 tmr_startms((tmr_t*)(n_player->tmr_duracion_nota), n_player->duracion_nota_actual);

	piLock(STD_IO_BUFFER_KEY);
			printf( "[NOTA %d][FREC %d][DURA %d]\n",
					n_player -> posicion_nota_actual +1,
					n_player -> frecuencia_nota_actual,
					n_player -> duracion_nota_actual);

			piUnlock(STD_IO_BUFFER_KEY);


}

void ActualizaPlayer (fsm_t* this) {

	TipoPlayer *p_player = (TipoPlayer*)this->user_data;


	piLock(PLAYER_FLAGS_KEY);
	flags_player &= ~ FLAG_NOTA_TIMEOUT;
	piUnlock(PLAYER_FLAGS_KEY);

	p_player -> posicion_nota_actual++;

 if (p_player->posicion_nota_actual < p_player->p_efecto->num_notas){
	tmr_startms((tmr_t*)(p_player->tmr_duracion_nota), p_player->duracion_nota_actual);

	p_player-> duracion_nota_actual=p_player->p_efecto->duraciones[p_player->posicion_nota_actual];
	p_player-> frecuencia_nota_actual=p_player->p_efecto->frecuencias[p_player->posicion_nota_actual];

		piLock(STD_IO_BUFFER_KEY);
		printf( "[NUEVA NOTA %d DE %d]\n",
			p_player -> posicion_nota_actual+1,
			p_player -> p_efecto ->num_notas);
			fflush(stdout);
		piUnlock(STD_IO_BUFFER_KEY);
}

else {
	piLock(PLAYER_FLAGS_KEY);
	flags_player |= FLAG_PLAYER_END;
	piUnlock(PLAYER_FLAGS_KEY);
	}
}


void FinalEfecto (fsm_t* this) {
	piLock(PLAYER_FLAGS_KEY);
		flags_player &= ~FLAG_PLAYER_END;
		piUnlock(PLAYER_FLAGS_KEY);


	softToneWrite(PLAYER_PWM_PIN,0);

		piLock(STD_IO_BUFFER_KEY);
		printf( "[se han reproducido todas las notas]\n ");
		piUnlock(STD_IO_BUFFER_KEY);
}

//------------------------------------------------------
// PROCEDIMIENTOS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void timer_player_duracion_nota_actual_isr (union sigval value) {
	// A completar por el alumno...

	piLock(PLAYER_FLAGS_KEY);
	flags_player |=  FLAG_NOTA_TIMEOUT;
	piUnlock(PLAYER_FLAGS_KEY);
	fflush(stdout);
}

