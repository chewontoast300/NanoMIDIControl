/*
	Arduino Nano Midi Controller
	Created by avalois2@protonmail.com on 2019-05-28
  Last Revision 2020-10-07
	Oled 128x32 display connected to i2c bus
*/
float versionNumber = 0.051; //For tracking versions

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool menuOn = false;
bool userInput = false;
bool screenChanged = true;
int menuSelect = 0;
unsigned long start = millis();
unsigned long previousMillis = 0;
unsigned long menuTimeOut = 15000;  // time in ms. to return to main screen if no input
int inputTimeOut = 5000;  // time in ms. to keep last user input on screen

// physical connections
int ROWS[8] = {2,3,4,5,6,7,8,9};
const int rowCount = sizeof(ROWS)/sizeof(ROWS[0]);
int COLS[3] = {10,11,12};
const int colCount = sizeof(COLS)/sizeof(COLS[0]);
byte keys[colCount][rowCount];
int POTS[2] = {A6,A7};
const int potCount = sizeof(POTS)/sizeof(POTS[0]);
struct Pot {
    byte pin;
    int threshold;
    int lastReading;
    int currentReading;
};
// defining an array of 2 Pots, one with pin A6 and threshold 5, the
// other with pin A7 and threshold 5. Everything else is automatically
// initialized to 0 (i.e. lastReading, currentReading). The order that
// the fields are entered determines which variable they initialize, so
// {A1, 4, 5} would be pin = A1, threshold = 4 and lastReading = 5
  struct Pot pots[] = { {A6, 5}, {A7, 5} };
int potRead;
int potValue;

int guiStatus = 0;  // for tracking display window

    // Set pins dependant on the int ROWS/COLS/POTS listed above.
    // Edit those pins and they will be mapped automatically.
void setPins() {
    for(int x=0; x<rowCount; x++) {
        Serial.print(ROWS[x]); Serial.println("set as INPUT");
        pinMode(ROWS[x], INPUT);
    }
    for (int x=0; x<colCount; x++) {
        Serial.print(COLS[x]); Serial.println("set as INPUT-Pullup");
        pinMode(COLS[x], INPUT_PULLUP);
    }
    for(int x=0; x<potCount; x++) {
        Serial.print(POTS[x]); Serial.println("set as Analog Input");
        pinMode(POTS[x], INPUT);
    } 
}


//-------------------- Display Functions Start ---------------//

    // D01 Screen startup and info
void showInit()
{
  delay(100);  // This delay is needed to let the display to initialize
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with the I2C address of 0x3C
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setRotation(0);  // Set orientation. Goes from 0, 1, 2 or 3
  display.setTextWrap(false);
  display.dim(1);  //Set brightness (0 is maximun and 1 is a little dim)
  display.setCursor(18, 8);
  display.print("NanoMidi V");
  display.print(versionNumber);
  display.setCursor(18, 20);
  display.print("Initialising");
  delay(1000);
  display.print(".");
  delay(1000);
  display.print(".");
  delay(1000);
  display.print(".");
  display.display();  // Print everything we set previously
}

    // D02 Function for controlling when screen needs to be changed. Keeps from having to refresh all the time.
void screenUpdate() {
  // keeps screen from refreshing if not needed
    display.clearDisplay();
    if (false == menuOn) {
      printMainScreen();
    } else {
      printMenuScreen();
    }
    display.display();  // Print everything we set previously
    screenChanged = false;
}

    // D03 keeps last control output on screen untill inputTimeOut
void displayUserInput() {
      unsigned long currentMillis = millis();
      if ((unsigned long)(currentMillis - previousMillis) >= inputTimeOut) {
        userInput = false;
        previousMillis = currentMillis;
       } else {
        Serial.println("printing input text");
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(10,14); //set cursor to Button display
      // display Button Output
        display.setTextSize(0);
        display.setCursor(60,12); //set cursor to Pot display
        display.println(potRead);
        display.setTextSize(1);
        display.setCursor(60,22); //set cursor to Pot display
        display.println(potValue);
        Serial.println(potRead);
        Serial.println(potValue);
  }
}


    // D04 Main info screen for controller
void printMainScreen() {
    //display.clearDisplay();  // Clear the display so we can refresh
    display.setTextSize(0);
    display.fillRect(0,0,128,10, WHITE);
    display.drawRect(0,0,128,32, WHITE);
    display.setCursor(3, 2);  // (x,y)
    display.setTextColor(BLACK);
    display.println("Button");
    display.setCursor(57, 2);
    display.println("Pot");
    display.setCursor(103, 2);
    display.println("100%");
    
    if (true == userInput) {
      displayUserInput();
      
    }
    /*
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10,14); //set cursor back to display button text
    */
}

    // D05 Main Menu for controller
void printMenuScreen() {
  Serial.println("Starting printMenuScreen");
    unsigned long now = millis();
    unsigned long elapsed = now - start;
    Serial.println(elapsed);   
    if ( elapsed < menuTimeOut) {
    //menuSelect = 0;
    display.drawRect(0,0,128,32, WHITE);
    display.drawLine(32,0,32,32, WHITE);
    display.drawLine(64,0,64,32, WHITE);
    display.drawLine(96,0,96,32, WHITE);
    menuSelections();
    menuControls();
    } else {
      menuOn = false;
  }
  Serial.println("Ending printMenuScreen ");
}

    // D06 List of menu items in menu
void menuSelections() {
  // show selected tile
    if (menuSelect == 0) {
    display.fillRect(0,0,32,32, WHITE);
    display.setTextSize(0);
    display.setTextColor(BLACK);
    display.setCursor(8, 12);
    display.println("Esc");
    display.setTextColor(WHITE);
    display.setCursor(38, 12);
    display.println("Midi");
    display.setCursor(72, 12);
    display.println("Cog");
    display.setCursor(100, 12);
    display.println("Info");
  } else if (menuSelect == 1) {
    display.fillRect(32,0,32,32, WHITE);
    display.setTextSize(0);
    display.setTextColor(BLACK);
    display.setCursor(38, 12);
    display.println("Midi");        
    display.setTextColor(WHITE);
    display.setCursor(8, 12);
    display.println("Esc");
    display.setCursor(72, 12);
    display.println("Cog");
    display.setCursor(100, 12);
    display.println("Info");
  } else if (menuSelect == 2) {
    display.fillRect(64,0,32,32, WHITE);
    display.setTextSize(0);
    display.setTextColor(BLACK);
    display.setCursor(72, 12);
    display.println("Cog");
    display.setTextColor(WHITE);
    display.setCursor(8, 12);   
    display.println("Esc");
    display.setCursor(38, 12);
    display.println("Midi");
    display.setCursor(100, 12);
    display.println("Info");
  } else if (menuSelect == 3)  {
    display.fillRect(96,0,32,32, WHITE);
    display.setTextSize(0);
    display.setTextColor(BLACK);
    display.setCursor(100, 12);
    display.println("Info");
    display.setTextColor(WHITE);
    display.setCursor(8, 12);   
    display.println("Esc");
    display.setCursor(38, 12);
    display.println("Midi");
    display.setCursor(72, 12);
    display.println("Cog");          
  }

  
}

    // D07 Maps controls for us in Menu Screen
void menuControls() {
  
}

//-------------------- Display Functions End -----------------//

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Up Nano Midi");
    Serial.print("V:");
    Serial.println(versionNumber);
    showInit();
    readAllThePots(pots);
    delay(3000); // for showing message
    setPins();
    screenUpdate();
    Serial.println("End Setup");
}


void readMatrix() {
    // iterate the columns
    for (int colIndex=0; colIndex < colCount; colIndex++) {
        // col: set to output to low
        byte curCol = COLS[colIndex];
        pinMode(curCol, OUTPUT);
        digitalWrite(curCol, LOW);
 
        // row: interate through the rows
        for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
            byte rowCol = ROWS[rowIndex];
            pinMode(rowCol, INPUT_PULLUP);
            keys[colIndex][rowIndex] = digitalRead(rowCol);
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }
}
 
void printMatrix() {
    for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
        if (rowIndex < 10)
            Serial.print(F("0"));
        Serial.print(rowIndex); Serial.print(F(": "));
 
        for (int colIndex=0; colIndex < colCount; colIndex++) {  
            Serial.print(keys[colIndex][rowIndex]);
            if (colIndex < colCount)
                Serial.print(F(", "));
        }   
        Serial.println("");
    }
    Serial.println("");
}

void readAllThePots(struct Pot * pot) {
    for(int i = 0; i < potCount; i++) {
        int reading = map(analogRead(pot[i].pin), 0, 664, 0, 200);

        if(abs(reading - pot[i].lastReading) >= pot[i].threshold) {
            //screenChanged = true;
            userInput = true;
            pot[i].currentReading = (reading/2);
            potRead = i;
            potValue = (reading/2);
            //previousMillis = start;
            displayUserInput();
            pot[i].lastReading = reading;
        }
    }
}

void loop() {
  //unsigned long currentMillis = millis();
  screenUpdate();
  /*
  if (screenChanged == true) {
    screenUpdate();
  }
  */
  //For MIDI Controller

    readAllThePots(pots);
    delay(10);
  /*
  readMatrix();
  printMatrix();
  delay(100);
  */
/* 
	readMatrix();
    if (Serial.read()=='!')
        printMatrix();
*/
/*	// Scan ROWS & COLS OLD WAY

  digitalWrite(ROWS[0],HIGH);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("C");
    display.println("C");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("Ab");
    display.println("Ab");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("FUNC5");
    display.println("FUNC5");
  }else{;}
  delay(15);

	// Scan Second ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],HIGH);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("C#");
    display.println("C#");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("A");
    display.println("A");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("FUNC6");
    display.println("F6");
  }else{;}
  delay(15);


	// Scan Third ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],HIGH);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("D");
    display.println("D");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("Bb");
    display.println("Bb");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("Unused");
    display.println("NULL1");
  }else{;}
  delay(15);

	// Scan Fourth ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],HIGH);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("Eb");
    display.println("Eb");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("B");
    display.println("B");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("Unused");
    display.println("NULL2");
  }else{;}
  delay(15);


	// Scan Fifth ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],HIGH);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("E");
    display.println("E");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("FUNC1");
    display.println("F01");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("Unused");
    display.println("NULL3");
  }else{;}
  delay(15);


	// Scan Sixth ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],HIGH);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("F");
    display.println("F");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("FUNC2");
    display.println("F02");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("Unused");
    display.println("NULL4");
  }else{;}
  delay(15);


	// Scan Seventh ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],HIGH);
  digitalWrite(ROWS[7],LOW);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("F#");
    display.println("F#");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("FUNC3");
    display.println("F03");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("Unused");
    display.println("NULL5");
  }else{;}
  delay(15);


	// Scan Eighth ROW
  digitalWrite(ROWS[0],LOW);
  digitalWrite(ROWS[1],LOW);
  digitalWrite(ROWS[2],LOW);
  digitalWrite(ROWS[3],LOW);
  digitalWrite(ROWS[4],LOW);
  digitalWrite(ROWS[5],LOW);
  digitalWrite(ROWS[6],LOW);
  digitalWrite(ROWS[7],HIGH);

  if(digitalRead(COLS[0]) == HIGH && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == LOW){
    Serial.println("G");
    display.println("G");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == HIGH && digitalRead(COLS[2]) == LOW){
    Serial.println("FUNC4");
    display.println("F04");
  }else if(digitalRead(COLS[0]) == LOW && digitalRead(COLS[1]) == LOW && digitalRead(COLS[2]) == HIGH){
    Serial.println("Unused");
    display.println("NULL6");
  }else{;}
  delay(15);

*/
   
}
