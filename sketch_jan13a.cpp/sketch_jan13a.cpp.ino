#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

// Pin configuration
#define RX_PIN 2
#define TX_PIN 3
#define BUZZER_PIN 8
#define LOCK_PIN 7


SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Variables
int attempts = 0;
String Id_string;

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LOCK_PIN, OUTPUT); // Set pin 8 as OUTPUT

  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  // Initialize fingerprint sensor
  finger.begin(57600);
  delay(5);

  // Check if fingerprint sensor is connected
  if (!finger.verifyPassword())
  {
    Serial.println("Fingerprint sensor not found!");
    while (1)
      ;
  }

  Serial.println("Fingerprint sensor found!");
  Serial.println("Place your finger on the sensor...");
}

void loop()
{
    delay(50);  
  // Wait for fingerprint
  if (finger.getImage() == FINGERPRINT_OK)
  {
    Serial.println("Image taken");

    // Convert fingerprint image to template
    if (finger.image2Tz() == FINGERPRINT_OK)
    {
      Serial.println("Image converted");

      // Search for a matching fingerprint
      if (finger.fingerFastSearch() == FINGERPRINT_OK)
      {
        // Fingerprint match found
        Id_string = String(finger.fingerID);
        String string1 = "Access granted, Welcome ID:" + Id_string;
        activateLock(1000); // Activate the buzzer on pin 8 for 1 second
        digitalWrite(LOCK_PIN, HIGH); // Deactivate the buzzer by setting it to LOW
        scrollText(string1, 500);
        delay(5000);
        lcd.clear();
        attempts = 0; // Reset failed attempts
      }
      else
      {
        // Fingerprint match not found
        String string2 = "Access Denied, try again";
       // lcd.setCursor(0,0);
       // lcd.print(string2);
       if (attempts < 2){
        scrollText(string2, 500);
        delay(3000);
        lcd.clear();
        attempts++; // Increment failed attempts
       }
        if (attempts == 2)
         {
            activateBuzzer();
            String string3 = "Access Denied, Alarm System Activated!";
            scrollText(string3, 500);
            delay(5000);
            lcd.clear();
            attempts = 0; // Reset the counter after activating the buzzer
         }
      }
    }
  }

  delay(1000); // Wait for a moment before trying again

  // Your code here to check for failed attempts
}
void scrollText(const String& text, unsigned long scrollDelay)
{
  int textLength = text.length();
  int displayLength = 16;
  int scrollPos = 0;
  unsigned long lastScrollTime = 0;
  unsigned long startTime = millis(); // Start time
  
  while (scrollPos <= textLength - displayLength)
  {
    unsigned long currentTime = millis();
    
    if (currentTime - lastScrollTime >= scrollDelay)
    {
      lastScrollTime = currentTime;
      lcd.clear();
      
      // Get the substring to display based on the current scroll position
      String substring = text.substring(scrollPos, scrollPos + displayLength);
      
      // Set the cursor to the first column of the first row
      lcd.setCursor(0, 0);
      
      // Display the substring on the first row
      lcd.print(substring);
      
      // Set the cursor to the first column of the second row
      //lcd.setCursor(0, 1);
      
      // Display the remaining characters on the second row
      lcd.print(text.substring(scrollPos + displayLength));
      
      scrollPos++;
    }
  }
}
void activateBuzzer()
{
  digitalWrite(BUZZER_PIN, HIGH); // Activate the buzzer by setting it to HIGH
  delay(10000); // Buzzer activation duration
  digitalWrite(BUZZER_PIN, LOW); // Deactivate the buzzer by setting it to LOW
}
void activateLock(unsigned long duration)
{
  digitalWrite(LOCK_PIN, LOW); // Activate the buzzer by setting it to HIGH
  delay(duration); // Wait for the specified duration
}
