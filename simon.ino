#include "DigitalTubeNumbers.h"

#define CLK 10 //pins definitions for TM1637 and can be changed to other ports
#define DIO 9

#define tiempoNota 400
#define tiempoNotaUser 300
#define tiempoEntreNotas 100
#define pinEsperaEntrada 13

#define periodoTonoFinPartida 4700
#define tiempoFinPartida 1200

#define pinBuzzer 8

byte pinLedTono[] = {A5,A4,A3,A2};
byte pinPulsador[] = {2,3,4,5};
int periodoTono[] = {3830,3038,2550,2028};
byte secuencia[] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};

byte numTonos = 0;
byte record = 0;
DigitalTubeNumbers tube(CLK,DIO);


void setup()
{
        Serial.begin(9600);
        
	for (int i = 0; i < 4; i++) {
		pinMode(pinLedTono[i], OUTPUT);
		pinMode(pinPulsador[i], INPUT);
	}
	
	pinMode(pinEsperaEntrada, OUTPUT);
	pinMode(pinBuzzer, OUTPUT);
	
        tube.init();
        tube.set(1);
        tube.setZerosLeft(false);
}


void loop()
{
	boolean seguir = true;
	numTonos = 0;
        digitalWrite(pinEsperaEntrada, HIGH);
        esperarInicio();
        digitalWrite(pinEsperaEntrada, LOW);
	tube.showNumber(record);
	delay(1000);

	while (seguir) {
	
		// metemos un nuevo tono en la secuencia
		secuencia[numTonos] = random(4);
		numTonos++;
                //printSecuencia();
		
		reproducirSecuencia();
		
		digitalWrite(pinEsperaEntrada, HIGH);
		seguir = leerSecuencia();
		digitalWrite(pinEsperaEntrada, LOW);
		
		if (!seguir) {
			finPartida();
		}
                else {
                  delay(1000);
                }
	}
}


void esperarInicio()
{
    byte patron[] = {0,1,2,3,2,1};
    
    while (true) {
        random(4);
        for (int i = 0; i < sizeof(patron); i++) {
            setLeds(LOW);
            digitalWrite(pinLedTono[patron[i]], HIGH);
            
            for (int j = 0; j < 2500; j++) {
                for (int k = 0; k < 4; k++) {
                    if (digitalRead(pinPulsador[k]) == HIGH) {
                        setLeds(LOW);
                        return;
                    }
                }
            }
        }
    }
}


void setLeds(boolean value)
{
    for (int i = 0; i < 4; i++) digitalWrite(pinLedTono[i], value);
}


void printSecuencia()
{
    Serial.print("secuencia: ");
    for (int i = 0; i < numTonos; i++) {
        Serial.print(secuencia[i]);
    }
    Serial.println(" :");
}

boolean leerSecuencia()
{
        Serial.println("leyendo secuencia...");
	byte pulsaciones = 0;

	while (true) {	
		for (int i = 0; i < 4; i++) {
                        random(4);
			if (digitalRead(pinPulsador[i]) == HIGH) {
                                
                                if (i == secuencia[pulsaciones]) {
                                    playLedTono(i, tiempoNotaUser);
			        }
  			        else {
                                    Serial.println(" error");
				    return false;
                                }
				pulsaciones++;
                		if (pulsaciones == numTonos) {
                			return true;
                		}
			}
		}
	}
}


void reproducirSecuencia()
{	
        Serial.print("reproducir secuencia: ");
	for (byte i = 0; i < numTonos; i++) {
		//Serial.print(secuencia[i]);
		playLedTono(secuencia[i], tiempoNota);
                delay(tiempoEntreNotas);
	}
	Serial.println();
}
		
		
		
void playLedTono(byte tono, unsigned long duration)
{
        unsigned long periodo = (unsigned long) periodoTono[tono];
        
	digitalWrite(pinLedTono[tono], HIGH);
        reproducirTono(periodo, duration);
	digitalWrite(pinLedTono[tono], LOW);
}

	
void reproducirTono(unsigned long periodo, unsigned long duration)
{
    /*
    Serial.print("periodo,duracion=");
    Serial.print(periodo);
    Serial.print(" ");
    Serial.println(duration);
    */
    unsigned long d = duration * 1000lu;
    unsigned long mitadPer = periodo / 2;
    unsigned long elapsed_time = 0;
	
    while (elapsed_time < d) {
        digitalWrite(pinBuzzer, HIGH);
        delayMicroseconds(mitadPer);
        digitalWrite(pinBuzzer, LOW);
        delayMicroseconds(mitadPer);
        elapsed_time += periodo;
    }
}

	
	
void finPartida()
{
        Serial.println("Fin partida");
	reproducirTono(periodoTonoFinPartida, tiempoFinPartida);
	int puntos = numTonos - 1;

	if (puntos > record) {
		for (int i = 0; i < 10; i++) {
			tube.showNumber(puntos);
			delay(150);
			tube.clearDisplay();
			delay(150);
		}
		record = puntos;
	}
	tube.showNumber(puntos);
}

