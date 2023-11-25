/*
  // FAST METHOD
  // REQUIRES PERIODIC UPDATE OF FINGERPRINT (EVERY 1 YEAR)
  // OUTDATED FINGERPRINT WILL STOP THE SERVICE
  // LATEST FINGERPRINT CAN BE FOUND AT- https://www.grc.com/fingerprints.htm?domain=test.firebaseio.com
  // REPLACE COLONS(:) WITH A SPACE(" ") AS SHOWN BELOW
  // 03:9E:4F:E6:83:FC:40:EF:FC:B2:C5:EF:36:0E:7C:3C:42:20:1B:8F ==> 03 9E 4F E6 83 FC 40 EF FC B2 C5 EF 36 0E 7C 3C 42 20 1B 8F
  #define FINGERPRINT "03 9E 4F E6 83 FC 40 EF FC B2 C5 EF 36 0E 7C 3C 42 20 1B 8F"
  Firebase firebase(PROJECT_ID, FINGERPRINT);
  // UNCOMMENT THIS BLOCK AND COMMENT THE LINE BELOW
*/

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Configuración de la base de datos de Firebase
#define FIREBASE_HOST "aerofresas0117-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "65efea2a8b5d543568e9879f70b09c15e0655293"

// Configuración de la red WiFi
#define WIFI_SSID "NOTE10+"
#define WIFI_PASSWORD "2023ABRIL"

// Variables para el manejo de datos desde Serial
char inputBuffer[100];
int inputIndex = 0;
bool newData = false;
String message = "";

bool pulse = false;

//Test
//String message = "{"tempIn": 998,"humdIn": 959,"tempEx": 24,"humdEx": 63,"vent": false,"humi": false,"air": false,"pulse": false}";

/* 
// Variables para almacenar datos
int tempIn = 0;
int humdIn = 0;
int tempEx = 0;
int humdEx = 0;

unsigned int on_vent = 0;
unsigned int on_humi = 0;
unsigned int on_air = 0;
*/

//Variables para el control del tiempo
unsigned long tiempoActual = 0;
unsigned long tiempoPrevio = 0;
const unsigned long intervalo = 20000; // Intervalo de tiempo para enviar datos a Firebase

void setup() {
  Serial.begin(9600);
  // Conexión a la red WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando");
    delay(500);
  }
  Serial.println("Conectado");

  // Inicialización de Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Habilitar el Watchdog Timer con una temporización de 8 segundos
  //ESP.wdtEnable(WDTO_8S);
}

void loop() {
  // Lectura de datos desde Serial
  while (Serial.available()) {
    char inChar = Serial.read();
    if (inChar == '{') {
      inputIndex = 0;
      newData = true;
      pulse = true;
    } else if (newData) {
      if (inChar == '}') {
        inputBuffer[inputIndex] = '\0'; // Null-terminate the string
        sendDataToFirebase(inputBuffer); // Procesar los datos recibidos
        sendPulseToFirebase(); // Enviar datos a Firebase
        tiempoPrevio = millis();
        newData = false;
        pulse = false;
      } else {
        inputBuffer[inputIndex] = inChar;
        inputIndex++;
      }
    }
  }

  //Control de tiempo para enviar datos a Firebase
  tiempoActual = millis();
  if (tiempoActual - tiempoPrevio >= intervalo) {
    tiempoPrevio = tiempoActual;
    sendPulseToFirebase(); // Enviar datos a Firebase
  }

  // Alimentar al Watchdog Timer para evitar reinicio
  //ESP.wdtFeed();
}

// Enviar datos a Firebase
void sendDataToFirebase(const char *message) {
  Firebase.setString("message", message);
  Serial.println(message);
  // Manejo de errores
  if (Firebase.failed()) {
      Serial.print("setting /message failed: ");
      Serial.println(Firebase.error());  
  } else {
      Serial.println("message sent to Firebase");
  }
}

// Enviar datos a Firebase
void sendPulseToFirebase(){
  // set bool value
  Firebase.setBool("aerofresas", pulse);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /aerofresas failed:");
      Serial.println(Firebase.error());  
  } else {
      Serial.println("aerofresas sent to Firebase");
  }
}

/*// Procesar los datos recibidos desde Serial
void processSerialData(const char *data) {
  int itemsParsed = sscanf(data, "#%d/%d/%d/%d/%u/%u/%u", 
                           &tempIn, &humdIn, &tempEx, &humdEx, 
                           &on_vent, &on_humi, &on_air);

  // Almacenar el mensaje recibido
  message = data;
}*/

/*// Almacenar la cadena `message` en la base de datos Firebase bajo la ruta "/logs"
  bool sendOK = Firebase.pushString("logs", message);
  // handle error
  if (Firebase.failed()) {
    Serial.print("pushing /logs failed: ");
    Serial.println(Firebase.error());
  } else {
    Serial.print("pushed: /logs/");
    Serial.println(sendOK);
  }*/