// Librerías 

#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>

// Instanciando

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(10, 9);

// Pines

int ledVerde = 5;
int ledRojo = 6;
int buzzer = 7;

// Variables

char clave[4] = {'1', '2', '3', '4'}; 
String tagUID = "9D E6 62 8B";  // Tag ingresado previamente
char clave_ingresada[4];   
boolean MODO_RFID = true; 
char tecla_presionada = 0; 
int contador = 0;  
const byte filas = 4;
const byte columnas = 4;
int intentos = 0;
int limite = 3;

// Configuracion del teclado

char teclas[filas][columnas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Pines teclado

byte filas_pines[filas] = {A0, A1, A2, A3};
byte columnas_pines[columnas] = {2, 3, 4};

// Instancia del teclado

Keypad teclado = Keypad( makeKeymap(teclas), filas_pines, columnas_pines, filas, columnas);

// Setup

void setup() {
  // Configuracion de pines
  
  pinMode(buzzer, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  lcd.init();   
  lcd.backlight();
  SPI.begin();      
  mfrc522.PCD_Init();   
  lcd.clear(); 
}

// Loop

void loop() {

  if (MODO_RFID == true) {
    lcd.setCursor(0, 0);
    lcd.print("   Bienvenido");
    lcd.setCursor(0, 1);
    lcd.print(" Coloque el tag ");
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    //Leyendo la tarjeta
    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();
    //Chequea el valor de la tarjeta
    if (tag.substring(1) == tagUID)
    {
      
      lcd.clear();
      digitalWrite(ledVerde, HIGH);
      tone(buzzer, 440);
      lcd.print("Ingresar clave:");
      lcd.setCursor(0, 1);
      MODO_RFID = false; 
      delay(500);
      noTone(buzzer);
      delay(1000);
      digitalWrite(ledVerde, LOW);
    }
    else
    {
      if(intentos>=2){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alerta policia");
        lcd.setCursor(0, 1);
        lcd.print("Intrusos");
        digitalWrite(buzzer, HIGH);
        digitalWrite(ledRojo, HIGH);
        delay(15000);
        digitalWrite(buzzer, LOW);
        digitalWrite(ledRojo, LOW);
      }
      intentos++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tag incorrecto");
      lcd.setCursor(0, 1);
      lcd.print("Acceso denegado");
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledRojo, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
      digitalWrite(ledRojo, LOW);
      lcd.clear();
    }

  }

  if (MODO_RFID == false) {
    tecla_presionada = teclado.getKey(); // Guardando tecla presionada
    if (tecla_presionada)
    {
      clave_ingresada[contador++] = tecla_presionada; // Guardando clave
      tone(buzzer, 330);
      _delay_ms(50);
      noTone(buzzer);
      digitalWrite(buzzer, LOW);
      lcd.print("*");
    }
    if (contador == 4) // Si 4 teclas fueron presionadas..
    {
      delay(200);
      if (!(strncmp(clave_ingresada, clave, 4))) // Chequea si valen lo mismo
      {
        lcd.clear();
        lcd.print("Acceso concedido");
        digitalWrite(ledVerde, HIGH);
        tone(buzzer, 440);
        delay(3000);
        digitalWrite(ledVerde, LOW);
        noTone(buzzer);
        lcd.clear();
        contador = 0;
        MODO_RFID = true;
      }
      else    // Si no valen lo mismo..
      {
        lcd.clear();
        lcd.print("Clave incorrecta");
        digitalWrite(buzzer, HIGH);
        digitalWrite(ledRojo, HIGH);
        delay(3000);
        digitalWrite(buzzer, LOW);
        digitalWrite(ledRojo, LOW);
        lcd.clear();
        contador = 0;
        MODO_RFID = true;
      }
    }
  }
}
