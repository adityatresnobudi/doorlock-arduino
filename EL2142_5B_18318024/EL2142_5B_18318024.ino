#include <Adafruit_Fingerprint.h>
#include <Servo.h> // menyertakan library servo ke dalam program
#include <Keypad.h>
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 12, 11, 10, 9 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 8, 7, 6 };
// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Servo myservo;     // variable untuk menyimpan posisi data
int electronic_lock = 13;
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(2, 3);// Tx, Rx
int ledRed = 4;
int ledGreen = 5;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

void setup()  
{
  myservo.attach(13);
  pinMode(13, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  myservo.write(0);
  Serial.println("Found fingerprint sensor");
  finger.getTemplateCount();
  Serial.print("Sensor contains "); 
  Serial.print(finger.templateCount); 
  Serial.println(" template(s)");
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    num = kpd.getKey();
  }
  return num;
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      delay(50);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      for (int i = 1; i <4; i++){
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        delay(300);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        delay(300); //LED merah dan hijau berkelap-kelip
      }
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  digitalWrite(ledRed, HIGH);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      digitalWrite(ledRed, LOW);
      for (int i = 1; i <4; i++){
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        delay(300);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        delay(300); //LED merah dan hijau berkelap-kelip
      }
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(50);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    delay(500);
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}

uint8_t saveFinger(){
  for (int i = 1; i <4; i++){
        digitalWrite(ledRed, HIGH);
        digitalWrite(ledGreen, HIGH);
        delay(300);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        delay(300); //LED merah dan hijau berkelap-kelip
      }
      digitalWrite(ledRed, HIGH);
      char pass1 = readnumber(); 
      Serial.println(pass1);
      char pass2 = readnumber(); 
      Serial.println(pass2);
      char pass3 = readnumber(); 
      Serial.println(pass3);
      //set sebagai password
      if (pass1 == '1'){
        if (pass2 == '2'){
          if (pass3 == '3'){ //set sebagai password
            digitalWrite(ledRed, LOW);
            for (int i = 1; i <4; i++){
            digitalWrite(ledGreen, HIGH);
            delay(300);
            digitalWrite(ledGreen, LOW);
            delay(300);} //LED hijau berkelap-kelip
            Serial.println("Ready to enroll a fingerprint!");
            Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
            delay(500);
            id = readnumber();
            Serial.print("Enrolling ID #");
            Serial.println(id);
            for (int i = 1; i <4; i++){
            digitalWrite(ledGreen, HIGH);
            delay(300);
            digitalWrite(ledGreen, LOW);
            delay(300);} //LED hijau berkelap-kelip
            getFingerprintEnroll();
          }
          else{
            for (int i = 1; i <4; i++){
              digitalWrite(ledRed, HIGH);
              delay(300);
              digitalWrite(ledRed, LOW);
              delay(300);}
          }
        }
        else{
          if (pass2 == '0' or pass2 == '1' or pass2 == '2' or pass2 == '3' or pass2 == '4' or pass2 == '5' or pass2 == '6' or pass2 == '7' or pass2 == '8' or pass2 == '9'){
            for (int i = 1; i <4; i++){
              digitalWrite(ledRed, HIGH);
              delay(300);
              digitalWrite(ledRed, LOW);
              delay(300);}
          }
        }
      }
      else {
        if (pass2 == '0' or pass2 == '1' or pass2 == '2' or pass2 == '3' or pass2 == '4' or pass2 == '5' or pass2 == '6' or pass2 == '7' or pass2 == '8' or pass2 == '9'){
          if (pass3 == '0' or pass3 == '1' or pass3 == '2' or pass3 == '3' or pass3 == '4' or pass3 == '5' or pass3 == '6' or pass3 == '7' or pass3 == '8' or pass3 == '9'){
            for (int i = 1; i <4; i++){
              digitalWrite(ledRed, HIGH);
              delay(300);
              digitalWrite(ledRed, LOW);
              delay(300);}
          }
          }
      }
 }

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  myservo.write(180);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, HIGH);
  Serial.println("Unlocked");
  delay(4000);
  myservo.write(0); //membuka pintu
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledGreen, LOW);
  delay(1000);
  return finger.fingerID; 
}

void loop()             // run over and over again
{
  char key = kpd.getKey(); 
  Serial.println(key);  
  Serial.println("Waiting for valid finger...");
  digitalWrite(ledRed, HIGH);
  getFingerprintIDez(); //mengecek sidik jari untuk membuka pintu (servo)
  delay(1);
  if (key == '*') {
  saveFinger(); //menyimpan sidik jari baru
  }
  }
