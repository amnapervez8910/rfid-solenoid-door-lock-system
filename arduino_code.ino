#include <SPI.h>
#include <MFRC522.h>

// Pin Definitions
#define RST_PIN 9
#define SS_PIN 10
#define RELAY_PIN 6
#define GREEN_LED 7
#define RED_LED 8
#define BUZZER_PIN 5

// Create RFID instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Authorized RFID Tag UID (change to your tag's UID)
byte authorizedUID[4] = {0xAB, 0xCD, 0x12, 0x34};  // Example UID

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, HIGH);  // Relay OFF (Lock engaged)
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  
  Serial.println("RFID Door Lock System Ready");
  Serial.println("Scan your RFID tag");
}

void loop() {
  // Check if new RFID card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select the card
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Read UID
  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Check if authorized
  if (checkUID(mfrc522.uid.uidByte)) {
    Serial.println("Access Granted! Door Unlocking...");
    accessGranted();
  } else {
    Serial.println("Access Denied! Wrong RFID Tag");
    accessDenied();
  }
  
  // Halt PICC
  mfrc522.PICC_HaltA();
  delay(500);
}

// Function to check UID
bool checkUID(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;
    }
  }
  return true;
}

// Function for authorized access
void accessGranted() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Activate relay (door unlocks)
  digitalWrite(RELAY_PIN, LOW);
  delay(3000);  // Door stays open for 3 seconds
  
  // Deactivate relay (door locks again)
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

// Function for unauthorized access
void accessDenied() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED, LOW);
}
