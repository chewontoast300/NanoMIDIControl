/*
	Arduino Nano Midi Controller
	Created by avalois2@protonmail.com on 2019-05-28
	Oled 128x32 display connected to i2c bus
*/
float versionNumber = 0.06; //For tracking versions

#include <SPI.h>
#include <Bounce2.h>
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
// bounce buttonDebounce[colCount][rowCount];
 char keyMap[colCount][rowCount] = {
  {'C',"C#",'D',"Eb",'E','F',"F#",'G'},
  {"Ab",'A',"Bb",'B',"X1","X2","X3","Y1"},
  {"Y2","Y3",0,0,0,0,0,0},
};
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
  display.display();
  delay(600);
  display.print(".");
  display.display();
  delay(600);
  display.print(".");
  display.display();
  delay(600);
  display.print(".");
  display.display();
}

    // D02 Function for controlling when screen needs to be changed.
    // Keeps us from having to refresh all the time.
void screenUpdate() {
    display.clearDisplay();
    if (false == menuOn) {
      printMainScreen();
    } else {
      printMenuScreen();
    }
    display.display();
    screenChanged = false;
}

    // D03 keeps last control output on screen untill inputTimeOut
void displayUserInput() {
      unsigned long currentMillis = millis();
      if ((unsigned long)(currentMillis - previousMillis) >= inputTimeOut) {
        userInput = false;
        previousMillis = currentMillis;
       } else {
        //Serial.println("printing input text");
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(10,14); //set cursor to Button display
        //display.print(keyMap[colIndex][rowIndex]);// display Button Output
        printMatrix();
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

void printMatrix() {
    for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
        //Serial.print(rowIndex); Serial.print(F(": "));
        for (int colIndex=0; colIndex < colCount; colIndex++) {  
            //Serial.print(keys[colIndex][rowIndex]);
            Serial.print(keyMap[colIndex][rowIndex]);
            //display.print(keyMap[colIndex][rowIndex]);
            if (colIndex < colCount)
                Serial.print(F(", "));
        }   
        Serial.println("");
    }
    Serial.println("");
    delay(500);
}

//-------------------- Display Functions End -----------------//

//------------------- Controls Function Start ----------------//

void scanPots(struct Pot * pot) {
    for(int i = 0; i < potCount; i++) {
        int reading = map(analogRead(pot[i].pin), 0, 664, 0, 200);

        if(abs(reading - pot[i].lastReading) >= pot[i].threshold) {
            //screenChanged = true;
            userInput = true;
            pot[i].currentReading = (reading/2);
            potRead = i;
            potValue = (reading/2);
            //previousMillis = start;
            //displayUserInput();
            pot[i].lastReading = reading;
        }
    }
}

void scanMatrix() {
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
            keyMap[colIndex][rowIndex] = digitalRead(rowCol);
              //Let's try setting the key pressed variable here if found to be low
            //if (digitalRead(rowCol) = 0) {
              //keyMap[colIndex][rowIndex] = keys[colIndex][rowIndex];
              
            //  userInput = true;
            //}
            pinMode(rowCol, INPUT);
        }
        // disable the column
        pinMode(curCol, INPUT);
    }
}

    // Maps controls for use in Menu Screen
void menuControls() {
  
}

//-------------------- Controls Function End -----------------//

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Up Nano Midi");
    Serial.print("V:");
    Serial.println(versionNumber);
    showInit();
    scanPots(pots);
    delay(2000); // for showing message
    setPins();
    screenUpdate();
    Serial.println("End Setup");
}

void loop() {
  
  //screenUpdate(); //for testing

      // Scan buttons and Pots for activity
    scanPots(pots);
    scanMatrix();
    //printMatrix();
      // Check for activity before updating anything on screen
  if (userInput == true) {
    screenUpdate();
}
    
    delay(10); // delay to keep loop from cycling too fast

  /*
  scanMatrix();
  printMatrix();
  delay(100);
  */
/* 
	scanMatrix();
    if (Serial.read()=='!')
        printMatrix();
*/
   
}
