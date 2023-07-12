#include <LiquidCrystal.h>
#include <Servo.h>
#include <IRremote.h>

// Definición de pines
#define TEMPERATURE_PIN A0
#define RS_PIN 12
#define EN_PIN 13
#define D4_PIN 5
#define D5_PIN 4
#define D6_PIN 3
#define D7_PIN 2
#define SERVO_PIN 9
#define LED_PIN1 6
#define LED_PIN2 7
#define POWER_BUTTON_PIN 10
#define Tecla_1 0xEF10BF00   
#define Tecla_2 0xEE11BF00
#define Tecla_3 0xED12BF00
#define Tecla_4 0xEB14BF00

// Constantes
const int TEMPERATURA_LIMIE = 60;
const int VERANO_TEMP_LIMITE = 25;
const int INVIERNO_TEMP_LIMITE = 10;
const int OTONO_TEMP_LIMITE = 13;
const int PRIMAVERA_TEMP_LIMITE = 16;

// Variables globales
IRrecv IR(11); //crea el sensor del IR
int flag = 1;
int temperaturaMaxima = 600; //Inicializo en un temp inalcanzable para que no se active
String estacion = "Configurar-> 1-4"; //mensaje por defecto
Servo servo;
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);
boolean estado_led1 = false;
boolean estado_led2 = false;
decode_results irResults;
int temperaturaAnterior = 0;
boolean incendioActivo = false;

void setup()
{
    lcd.begin(16, 2);

    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);

	IR.enableIRIn();
  
    Serial.begin(9600);

    servo.attach(SERVO_PIN);
}
void evaluarTemperatura() 
{
  	char cadenaTemperatura[16];
    int lectura, mostrar;
  
  	lectura = analogRead(A0);
  	mostrar = map(lectura, 20, 358, -40, 125); 
  	sprintf(cadenaTemperatura, "Temperatura: %d  ", mostrar);
	lcd.setCursor(0,0);
  	lcd.print(cadenaTemperatura);
  	if(mostrar > temperaturaMaxima) 
    {
      	lcd.setCursor(0, 1);
      	lcd.print ("         ");
      	lcd.setCursor(0, 1);
      	lcd.print("INCENDIO");
      	digitalWrite(LED_PIN1, HIGH);
      	digitalWrite(LED_PIN2, LOW);
      	delay(500);
      	incendioActivo = true;
    	servo.write(180); // Gira el servo a 180 grados
    }
  	else //Si la temperatura volvió a niveles normales
    {
      digitalWrite(LED_PIN2, HIGH);
      digitalWrite(LED_PIN1, LOW);
      lcd.setCursor(0,1);
      lcd.print(estacion);
      incendioActivo = false;
      servo.write(90); // Vuelve el servo a la posición inicial (90 grados)
    }
}


//Chequea si un boton entre 1 y 4 es presionado en el control
//En base a ello imprime la estación en el display y fija la temperatura máxima
void mostrarEstacion()
{
	if(IrReceiver.decode())
  	{
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

        switch (IrReceiver.decodedIRData.decodedRawData)
      {
          case Tecla_1:
              estacion = "PRIMAVERA       ";
              temperaturaMaxima = 25;
          break;
          case Tecla_2:
              estacion = "VERANO          ";
              temperaturaMaxima = 32;
          break;
          case Tecla_3:
              estacion = "OTONO           ";
              temperaturaMaxima = 24;
          break;
          case Tecla_4:
              estacion = "INVIERNO        ";
              temperaturaMaxima = 17;
          break;
          default:
          break;
      }
      
      lcd.setCursor(0,1);
      lcd.print(estacion);
      IR.resume();
  	}
  	else if(flag == 1) //Establece el mensaje por defecto al iniciar el sistema
    {
      lcd.setCursor(0,1);
      lcd.print(estacion);
      IR.resume();
  	  flag += 1;
    }
}
void loop()
{
    mostrarEstacion();
   	evaluarTemperatura();
}
