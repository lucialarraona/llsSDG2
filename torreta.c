
#include "torreta.h"
#include <SoftPwm.h>

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

int debounceTime_impacto=0;

void InicializaTorreta (TipoTorreta *p_torreta) {

	TipoServo *p_servox = &(p_torreta->servo_x);
	TipoServo *p_servoy = &(p_torreta->servo_y);

	wiringPiSetupGpio();

		p_servox->incremento = SERVO_INCREMENTO;
		p_servox->minimo 	= SERVO_MINIMO;
		p_servox->maximo 	= SERVO_MAXIMO;

		p_servox->inicio 	= SERVO_MINIMO + (SERVO_MAXIMO - SERVO_MINIMO)/2;
		p_torreta->  posicion.x = p_servox->inicio;

		if(p_torreta->posicion.x > p_servox->maximo)
			p_torreta->posicion.x = p_servox->maximo;

		if(p_torreta->posicion.x < p_servox->minimo)
			p_torreta->posicion.x =p_servox->minimo;

		softPwmCreate (SERVO_HORIZONTAL_PIN, p_servox->inicio, SERVO_PWM_RANGE); // Internamente ya hace: piHiPri (90) ;
		softPwmWrite(SERVO_HORIZONTAL_PIN, p_torreta->posicion.x);


		p_servoy->incremento = SERVO_INCREMENTO;
		p_servoy->minimo 	= SERVO_MINIMO;
		p_servoy->maximo 	= SERVO_MAXIMO;

		p_servoy->inicio 	= SERVO_MINIMO + (SERVO_MAXIMO - SERVO_MINIMO)/2;
		p_torreta->posicion.y = p_servoy->inicio;

		if(p_torreta->posicion.y > p_servoy->maximo)
			p_torreta->posicion.y = p_servoy->maximo;

		if(p_torreta->posicion.y < p_servoy->minimo)
			p_torreta->posicion.y =p_servoy->minimo;

		softPwmCreate (SERVO_VERTICAL_PIN, p_servoy->inicio, SERVO_PWM_RANGE); // Internamente ya hace: piHiPri (90) ;
		softPwmWrite(SERVO_VERTICAL_PIN, p_torreta->posicion.y);

		pinMode(IR_TX_PIN, OUTPUT);
        pinMode(IR_RX_PIN, INPUT);

		p_torreta -> tmr_duracion_disparo = tmr_new (timer_duracion_disparo_isr);

		wiringPiISR(IR_RX_PIN, INT_EDGE_RISING, ImpactoUP_isr);
	}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaComienzo (fsm_t* this) {

	int result = 0;
   result=(flags_juego & FLAG_SYSTEM_START);
	return result;
}

int CompruebaJoystickUp (fsm_t* this) {

	int result = 0;
	 result=(flags_juego & FLAG_JOYSTICK_UP);
	return result;
}

int CompruebaJoystickDown (fsm_t* fsm_player){

	int result = 0;
	result=(flags_juego & FLAG_JOYSTICK_DOWN);
	return result;
}

int CompruebaJoystickLeft (fsm_t* this) {

	int result = 0;
	result=(flags_juego & FLAG_JOYSTICK_LEFT);
	return result;
}

int CompruebaJoystickRight (fsm_t* this) {

	int result = 0;
	result=(flags_juego & FLAG_JOYSTICK_RIGHT);
	return result;
}

int CompruebaTimeoutDisparo (fsm_t* this) {

	int result = 0;
	result=(flags_juego & FLAG_SHOOT_TIMEOUT);
	return result;
}

int CompruebaImpacto (fsm_t* this) {

	int result = 0;
	result=(flags_juego & FLAG_TARGET_DONE);
	return result;
}

int CompruebaTriggerButton (fsm_t* this) {

	int result = 0;
	result=(flags_juego & FLAG_TRIGGER_BUTTON);
	return result;
}

int CompruebaFinalJuego (fsm_t* this) {

	int result = 0;
	result=(flags_juego & FLAG_SYSTEM_END);
	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void ComienzaSistema (fsm_t* this) {
	// A completar por el alumno

	TipoTorreta *p_torreta;
	p_torreta = (TipoTorreta*)(this->user_data);

	flags_juego=0;
	piLock(STD_IO_BUFFER_KEY);
	flags_juego &= (~ FLAG_SYSTEM_START);
	piUnlock(STD_IO_BUFFER_KEY);


	printf("comienza juego\n");
	fflush(stdout);

	InicializaTorreta(p_torreta);
}

void MueveTorretaArriba (fsm_t* this) {


	TipoTorreta *p_torreta;
	p_torreta = (TipoTorreta*)(this->user_data);

	flags_juego &= (~FLAG_JOYSTICK_UP);

	if(p_torreta->posicion.y + p_torreta->servo_y.incremento <= p_torreta->servo_y.maximo) {
	p_torreta->posicion.y = p_torreta->posicion.y + p_torreta->servo_y.incremento;

	softPwmWrite(SERVO_VERTICAL_PIN	, p_torreta->posicion.y);

	printf("[SERVO][POSICION]=[%d]\n", p_torreta->posicion.y);
	fflush(stdout);
		}


}

void MueveTorretaAbajo (fsm_t* this) {

	TipoTorreta *p_torreta;
	p_torreta = (TipoTorreta*)(this->user_data);

	flags_juego &= (~FLAG_JOYSTICK_DOWN);

	if(p_torreta->posicion.y - p_torreta->servo_y.incremento >= p_torreta->servo_y.minimo) {
	 p_torreta->posicion.y = p_torreta->posicion.y - p_torreta->servo_y.incremento;

	softPwmWrite(SERVO_VERTICAL_PIN	, p_torreta->posicion.y);

	printf("[SERVO][POSICION]=[%d]\n", p_torreta->posicion.y);
	fflush(stdout);
			}


}

void MueveTorretaIzquierda (fsm_t* this) {

	TipoTorreta *p_torreta;
	p_torreta = (TipoTorreta*)(this->user_data);

	flags_juego &= (~FLAG_JOYSTICK_LEFT);

	if(p_torreta->posicion.x - p_torreta->servo_x.incremento >= p_torreta->servo_x.minimo) {
	 p_torreta->posicion.x = p_torreta->posicion.x - p_torreta->servo_x.incremento;

	softPwmWrite(SERVO_HORIZONTAL_PIN, p_torreta->posicion.x);

	printf("[SERVO][POSICION]=[%d]\n", p_torreta->posicion.x);
	fflush(stdout);
			}


}

void MueveTorretaDerecha (fsm_t* this) {

	TipoTorreta *p_torreta;
	p_torreta = (TipoTorreta*)(this->user_data);

	flags_juego &= (~FLAG_JOYSTICK_RIGHT);

	if(p_torreta->posicion.x + p_torreta->servo_x.incremento <= p_torreta->servo_x.maximo) {
	p_torreta->posicion.x = p_torreta->posicion.x + p_torreta->servo_x.incremento;

	softPwmWrite(SERVO_HORIZONTAL_PIN, p_torreta->posicion.x);

	printf("[SERVO][POSICION]=[%d]\n", p_torreta->posicion.x);
	fflush(stdout);
				}

}

void DisparoIR (fsm_t* this) {

	TipoTorreta *p_torreta;
	p_torreta = (TipoTorreta*)(this->user_data);

	piLock(PLAYER_FLAGS_KEY);
	flags_player |= FLAG_START_DISPARO;
	piUnlock(PLAYER_FLAGS_KEY);

	piLock(SYSTEM_FLAGS_KEY);
	flags_juego &= ~FLAG_TRIGGER_BUTTON;
	piUnlock(SYSTEM_FLAGS_KEY);

	printf("encendemos\n");

	digitalWrite(IR_TX_PIN,HIGH);

	tmr_startms((tmr_t*)(p_torreta->tmr_duracion_disparo), SHOOTING_PERIOD);
}

void FinalDisparoIR (fsm_t* this) {

	piLock(SYSTEM_FLAGS_KEY);
	flags_juego &= (~FLAG_SHOOT_TIMEOUT);
	piUnlock(SYSTEM_FLAGS_KEY);

	digitalWrite(IR_TX_PIN,LOW);

}

void ImpactoDetectado (fsm_t* this) {

	piLock(PLAYER_FLAGS_KEY);
	flags_player |= FLAG_START_IMPACTO;
	flags_player &= ~FLAG_START_DISPARO;
	piUnlock(PLAYER_FLAGS_KEY);

	piLock(SYSTEM_FLAGS_KEY);
	flags_juego &= ~FLAG_TARGET_DONE;
	flags_juego &= ~FLAG_SHOOT_TIMEOUT;
	piUnlock(SYSTEM_FLAGS_KEY);

	digitalWrite(IR_TX_PIN,LOW);
printf("apagamos\n");
}


void ImpactoUP_isr(void){

	piLock(SYSTEM_FLAGS_KEY);
	flags_juego |= FLAG_TARGET_DONE;
	piUnlock(SYSTEM_FLAGS_KEY);

}


void FinalizaJuego (fsm_t* this) {

flags_juego = 0;

}
void timer_duracion_disparo_isr (union sigval value) {

	piLock(SYSTEM_FLAGS_KEY);
	flags_juego |= FLAG_SHOOT_TIMEOUT;
	piUnlock(SYSTEM_FLAGS_KEY);

	fflush(stdout);
}

void nulo(fsm_t* this){
	printf("vuelve");
	fflush(stdout);
}

int vuelve(fsm_t* this){
	return 1;
}

/*void rx_isr (void) {
	if (millis () < debounceTime_impacto) {
		debounceTime_impacto = millis () + DEBOUNCE_TIME ;
		return;
	}
	piLock(SYSTEM_FLAGS_KEY);
	flags_juego |= (FLAG_TARGET_DONE);
	piUnlock(SYSTEM_FLAGS_KEY);
	debounceTime_impacto = millis () + DEBOUNCE_TIME ;
	piLock (STD_IO_BUFFER_KEY);
	printf(" RECEPCION WORKS OK");
	piUnlock (STD_IO_BUFFER_KEY);
	fflush(stdout);


}*/
// el de la cosita es el DEL PIN
