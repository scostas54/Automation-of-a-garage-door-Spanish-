// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       Control_Portal.ino
    Created:	29/09/2019 22:41:52
    Author:     Samuel_Costas
*/
//Variables de control
byte estado_movimiento = 2; //Si 0 cerrando, si 1 abriendo, si 2 parado
byte parado_movimiento = 2; // NO = 0, SI = 1
byte DelayTotal = 200;
long last_bounce = 0;
long tiempo_apertura = 10000; //tiempo (en ms) de seguridad en el que el portal debe abrirse, si no se abre se cierra autom�ticamente.
long contador_inicio = 0; //contador del tiempo de apertura del portal

//Se definen las entrada
const byte E1 = 5; // micro portal cerrado
const byte E2 = 6; // micro portal abierto
const byte E3 = 7; // barrera infrarroja
const byte E4 = 8; // micro pulsador
const byte E5 = 9; // mando a distancia
const byte E6 = 10; // micro velocidad lenta cerrando
const byte E7 = 12; // micro velocidad lenta abriendo

//Se definen las salidas
const byte S1 = 2; //Rele k1
const byte S2 = 4; //Rele k2
const byte S3 = 3; //Rele k3
const byte S4 = 11; //Rele k4, para velocidad lenta

/* La funcion setup() se ejecuta una vez el microcontrolador arranca, en ella se inicializan todos los pines
y en esencia todo aquello que solo interesa que se ejecute una vez. void indica que la funcion no retorna nada
funcion vacia.*/
void setup()
{
	pinMode(E1, INPUT_PULLUP); /*Input_pullup, activa la resistencia interna de 20k del pin digital, de este modo
	leemos ALTO cuando el boton esta abierto. Cuando el boton esta cerrado, el Arduino 
	lee BAJO porque se ha completado una conexion a tierra. Input define el pin como entrada.*/
	pinMode(E2, INPUT_PULLUP);
	pinMode(E3, INPUT_PULLUP);
	pinMode(E4, INPUT_PULLUP);
	pinMode(E5, INPUT_PULLUP);
	pinMode(E6, INPUT_PULLUP);
	pinMode(E7, INPUT_PULLUP);

	pinMode(S1, OUTPUT); //Output define el pin como salida.
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(S4, OUTPUT);

	digitalWrite(S4, HIGH); //Se activa el rele k4
	digitalWrite(S3, HIGH); //Se activa el rele k3
	digitalWrite(S2, HIGH); //Se activa el rele k2
	digitalWrite(S1, HIGH); //Se activa el rele k1
		
}

// Add the main program code into the continuous loop() function
void loop()
{
	byte valor_E1 = digitalRead(E1);	//Estado portal CERRADO. lee el valor del pin asociado a E1 y almacena dicho valor en valor_E1
	byte valor_E2 = digitalRead(E2);	//Estado portal ABIERTO
	byte valor_E3 = digitalRead(E3);	//Estado BARRERA INFRARROJA
	byte valor_E4 = digitalRead(E4);	//Estado PULSADOR
	byte valor_E5 = digitalRead(E5);	//Estado MANDO A DISTANCIA
	byte valor_E6 = digitalRead(E6);	//Estado velocidad lenta cerrando
	byte valor_E7 = digitalRead(E7);	//Estado velocidad lenta abriendo
	
	//Debe reiniciarse el contador cada vez que se pare el portal
	if (estado_movimiento == 2) {
		contador_inicio = millis();
	}
	
	//CASOS MOVIMIENTO
	//Si se activa el mando o el pulsador, estado_movimiento = 2 (parado) y micro portal cerrado esta activo entonces se abre.
	if ((valor_E5 == 0 || valor_E4 == 0) && valor_E1 == 0 && estado_movimiento == 2 && ((millis() - last_bounce) > DelayTotal)) {
		digitalWrite(S2, LOW); //Se activa el rele k3, esta funcion pone el valor del pin asociado a S3 en alto.
		estado_movimiento = 1;
		parado_movimiento = 0;
		last_bounce = millis();
	}
	/*Si se activa el mando o el pulsador, estado_movimiento = 2 (parado) y micro portal abierto esta activo entonces se cierra, 
	tambien si se pulsan y se habia parado abriendo o cerrando (parado_movimiento == 1)*/
	else if (((valor_E5 == 0 || valor_E4 == 0) && valor_E2 == 0 && estado_movimiento == 2)
		|| (parado_movimiento == 1 && (valor_E4 == 0 || valor_E5 == 0) && ((millis() - last_bounce) > DelayTotal))) {
		digitalWrite(S3, LOW); //Se activa el rele k3
		digitalWrite(S2, LOW); //Se activa el rele k2
		digitalWrite(S1, LOW); //Se activa el rele k1
		estado_movimiento = 0;
		parado_movimiento = 0;
		last_bounce = millis();
	}
	/*Si se activa el micro de velocidad lenta se cambia la posicion del rele k4 para que active la velocidad lenta*/
	else if ((valor_E6 == 0 || valor_E7 == 0) && estado_movimiento != 2 && ((millis() - last_bounce) > DelayTotal)) {
		digitalWrite(S4, LOW); //Se activa el rele k4		
		last_bounce = millis();
	}
	//Si transcurre el tiempo marcado en tiempo_apertura sin que se cierre el portal, se cierra por defecto.
	else if ((millis() - contador_inicio >= tiempo_apertura) && valor_E3 == 1 && valor_E1 == 1) {
		digitalWrite(S3, LOW); //Se activa el rele k3
		digitalWrite(S2, LOW); //Se activa el rele k2
		digitalWrite(S1, LOW); //Se activa el rele k1
		estado_movimiento = 0;
		parado_movimiento = 0;		
	}

	//CASOS PARADA
	//Si se activa el micro de Portal cerrado se detiene el portal
	//Si se activa el micro de Portal abierto se detiene el portal
	if ((valor_E1 == 0 && estado_movimiento == 0) 
		|| (valor_E2 == 0 && estado_movimiento == 1)) {
		digitalWrite(S3, HIGH); //Se desactiva el rele k3
		digitalWrite(S2, HIGH); //Se desactiva el rele k2
		digitalWrite(S1, HIGH); //Se desactiva el rele k1
		digitalWrite(S4, HIGH); //Se desactiva el rele k4, la velocidad lenta
		estado_movimiento = 2;
	}
	//Si se activa la barrera y estado_movimiento = 0, se detiene y se abre
	else if (valor_E3 == 0 && estado_movimiento == 0) {
		digitalWrite(S3, HIGH); //Se desactiva el rele k3
		digitalWrite(S2, HIGH); //Se desactiva el rele k2
		digitalWrite(S1, HIGH); //Se desactiva el rele k1
		delay(2000); //Se para durante 2000ms
		digitalWrite(S2, LOW); //Se activa el rele k3, se abre el portal
		estado_movimiento = 1;	
	}
	//Si se activa el mando o pulsador y estado_movimiento != 2, se detiene y estado_movimiento guarda la direccion del portal
	else if (parado_movimiento == 0 && (valor_E4 == 0 || valor_E5 == 0) && ((millis() - last_bounce) > DelayTotal)) {			
		digitalWrite(S3, HIGH); //Se desactiva el rele k3
		digitalWrite(S2, HIGH); //Se desactiva el rele k2
		digitalWrite(S1, HIGH); //Se desactiva el rele k1	
		estado_movimiento ++;	
		parado_movimiento ++;	
		last_bounce = millis();
	}
	
}
