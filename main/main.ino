//note that actually if we want we can introduce 1 dp to make it more precise la 
//weight actually is a variable that defines the offset from desired value: weight= actual- desired 

// Libraries required 
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <PubSubClient.h>

// custom config files
#include "LoadCellConfig.h"
#include "KeypadConfig.h"
#include "DisplayLCDConfig.h"
#include "NeopixelLEDConfig.h"
#include "WifiConfig.h"
#include "GoogleSheetConfig.h"
#include "Recipe.h"
#include "DisplayRecipeInfo.h"
#include "MQTTConfig.h" 

// states for weighing scale
enum States { RECIPE_SELECTION, CONFIRMATION, WEIGHING };
States currentState;

//This was added as it will be difficult for this machine to make very precise measurement. 
//So if it falls within a certain range, we will still count that the measurement is desirable. 
//eg if weight is 200 and offset is 1 means acceptable weight is between 199g and 201g. 
int offset = 2;
int weight;
int oldWeight; // to minimise lcd panel updates 
String text; //for display text scrolling

// initialize HX711 load cell, LCD, LED, keypad
HX711 scale;
LiquidCrystal_I2C lcd(LCDADDRESS, LCDCOLUMNS, LCDROWS); 
Keypad kpd = Keypad(makeKeymap(KEYS), ROWPINS, COLPINS, ROWS, COLS); 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

std::vector<Recipe> recipeList; // store all google sheet recipes into this list
String selectedRecipeNum; // stores selected recipe number inputted by user
Recipe desiredRecipe; // stores selected recipe object

// stores current ingredient and its measurement
String ingredient;
int desiredWeight;

bool canGetNextIngredient = false; // indicate if users can move on to the next ingredient in the recipe

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient); // Declare the MQTT client globally
String previousIngredient = ""; // Add a flag to track if MQTT has been initialized and ingredient published


void setup() {
  Serial.begin(115200);
  
  //Code to slow down MCU 
  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);

  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);          
  scale.set_scale(CALIBRATION_FACTOR); 
  scale.tare(); // reset the scale to 0

  connectToWifi();
  lcdSetup();
  neopixelLEDSetup();

  /* 
      START OF MQTT SETUP SECTION 
  */ 

  // Initialize MQTT client
  mqttClient.setServer(MQTTServer, MQTTPort);
  mqttClient.setClient(espClient);

  // Set the CA certificate
  espClient.setCACert(MQTTCACert);

  if (mqttClient.connect(MQTTClientID, MQTTUsername, MQTTPassword)) {
    Serial.println("Connected to MQTT Broker");
  } else {
    Serial.print("MQTT Connection Failed. Error code: ");
    Serial.println(mqttClient.state());
  }

  /* 
      END OF MQTT SETUP SECTION 
  */ 


  // connect to google sheet containing recipes
  Serial.println("Retrieving recipes");
  HTTPClient http;
  http.begin(GOOGLE_SHEET_REQUEST_URL);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    // return the data from google sheets in JSON format
    String payload = http.getString();

    // parse JSON and extract values into the recipeList
    Recipe recipe;
    recipe.parseFromJSON(payload, recipeList);
  }

  //run once to store some value so that first loop will have oldWeight variable
  weight = scale.get_units();
  oldWeight = weight;

  Serial.println("Initialisation completed");

  // after setup, set to recipe selection (default) state
  currentState = RECIPE_SELECTION;
}

// main logic loop
void loop() {
  switch (currentState) {
    case RECIPE_SELECTION:
    {
      // when first on recipe selection screen, print prompt
      if (selectedRecipeNum == ""){
        // Serial.println("Select recipe");
        lcd.setCursor(0, 0);
        lcd.print("Select recipe");
      }

      char recipeSelectionKey = kpd.getKey(); //storing pressed key value in a char

      if (recipeSelectionKey != NO_KEY){ //no_key will be display when no buttons is pressed 
        recipeSelectionStateButtonInputs(recipeSelectionKey);
      };
      break;
    }
      
    
    case CONFIRMATION:
    {
      Serial.println ("Confirm Recipe? " + desiredRecipe.getRecipeName());
      //on autoscroll if necessary. if put "confirm selection", lcd format will have problem.maybe because quota is exceeded?
      displayConfirmRecipeOnLCD(("Confirm Recipe? "+ desiredRecipe.getRecipeName()));
      
      char confirmationKey = kpd.getKey();
      if (confirmationKey != NO_KEY){
        //off autoscroll of text when button is detected 
        lcd.noAutoscroll();
        confirmationStateButtonInputs(confirmationKey);
      }
      break;
    }

    case WEIGHING:
    {
      // Get the selected ingredient and weight
      ingredient = desiredRecipe.getCurrentIngredient();
      desiredWeight = desiredRecipe.getQuantityOfCurrentIngredient();

      // Loop to check the current ingredient and publish to MQTT if it is different from the previous ingredient
      if (ingredient != previousIngredient) {
        // Initialize MQTT client
        mqttClient.setServer(MQTTServer, MQTTPort);
        mqttClient.setClient(espClient);

        // Set the CA certificate
        espClient.setCACert(MQTTCACert);
        int mqttState = mqttClient.state();

        if (mqttClient.connect(MQTTClientID, MQTTUsername, MQTTPassword)) {
          Serial.println("Connected to MQTT Broker");

          const char* mqttTopic = "topic/Inventory"; 
          mqttClient.publish(mqttTopic, ingredient.c_str(), 1);
          Serial.println("Published ingredient: " + ingredient);

          // Update the previousIngredient to the current ingredient
          previousIngredient = ingredient;
        } else {
          Serial.print("MQTT Connection Failed. Error code: ");
          Serial.println(mqttClient.state());
        }
      }

      // output 1st ingredient and quantity on LCD
      Serial.println ("Ingredient: " + ingredient);
      Serial.println ("Desired Weight: ");
      Serial.print(desiredWeight);
    
      // get weight from weighing scale and find diff in weights  
      Serial.println("measured weight: ");
      Serial.print(weight);

      if (ingredient.length()<16){
        char weighingKey = kpd.getKey();
        if (weighingKey != NO_KEY){
          weighingStateButtonInputs(weighingKey);
        }
        weight = scale.get_units();
        weight = calculateWeightDiff(weight, desiredWeight);
        displayWeightOnLCD (ingredient, weight);
        if (weight != oldWeight){ //code over here tries to minimise updating time
          oldWeight = weight; // update old weight to new weight if reading is different
          // change LED colours based on diff in weight
          ledChange(weight, offset);
          lcd.clear();
          displayWeightOnLCD (ingredient, weight);
          delay(100);
        }
        //if detect correct weight 
        if ((weight >- 1 * offset) && weight < offset){
          canGetNextIngredient = true;
        }
      }
      else{
      //for case ingredient string too long, activate scroll.btw each update recheck for weight changes  
        for (int i=0; i<(ingredient.length()); i++){
          char weighingKey = kpd.getKey();
          if (weighingKey != NO_KEY){
            weighingStateButtonInputs(weighingKey);
          }
          weight = scale.get_units();
          weight = calculateWeightDiff(weight, desiredWeight);
          lcd.clear();//to clear screen to update changes 
          lcd.setCursor(0,0);//reset cursor to top left of screen
          lcd.print(ingredient.substring(i, ingredient.length()));
          if (weight != oldWeight){ //update weight part if weight is different
            oldWeight = weight; // update old weight to new weight if reading is different 
          } 
          displayWeightOnLCDonly(weight);//updateweight
          ledChange(weight,offset);
          //if detect correct weight 
          if ((weight >- 1 * offset) && weight < offset){
            canGetNextIngredient = true;
            break;
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

// Function for keypad buttons during recipe selection state
void recipeSelectionStateButtonInputs(char key){ 
  Serial.println(key);
  switch (key) {
    // button B - confirm recipe selection
    case 'B':
    {
      Serial.println ("Confirming...");
      // ensure that selectedRecipeNum is not empty
      if (selectedRecipeNum != "") {
        // check if recipe number is valid
        // if valid, proceed to confirmation state 
        try {
          desiredRecipe = Recipe::getByRecipeNo(selectedRecipeNum.toInt(), recipeList); 
          currentState = CONFIRMATION;
        }
        
        // if recipe number given is not in the list of recipes, have users select recipe again
        catch (const std::exception& e) {
          lcd.print(e.what());
          Serial.println (e.what());
          delay(500);
          
          selectedRecipeNum = "";
        }
      }
      
      else {
        // if recipe hasn't been selected yet, indicate to users
        Serial.println ("Select a recipe first!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Select a recipe first!");
      }
      break;
    }

    // button C - clears lcd display and resets selected recipe
    case 'C':
    {
      lcd.clear();
      Serial.println ("Clearing...");
      selectedRecipeNum = "";
      break;
    }

    // button D - delete last inputted number
    case 'D':
    {
      // remove last inputted character in selected recipe num
      selectedRecipeNum.remove(selectedRecipeNum.length() - 1);

      // clear lcd and display new selected recipe num
      Serial.println ("Deleting last input...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select recipe");
      lcd.setCursor(0, 1);
      lcd.print(selectedRecipeNum);
      break;
    }

    // numerical buttons - print number inputted to lcd and concatenate to selectedRecipeNum
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      lcd.setCursor(0, 1);
      lcd.print("                "); 
      // if selectedRecipeNum is empty, set the inputted number as recipeNum
      if (selectedRecipeNum.length() <= 0){
        selectedRecipeNum = key;
      } 
      // if selectedRecipeNum is not empty, concat to selectedRecipeNum
      else {
        selectedRecipeNum += key;
      }
      Serial.println ("Selected Recipe: " + selectedRecipeNum);
      lcd.setCursor(0, 1);
      lcd.print(selectedRecipeNum); // This line prints the entire selectedRecipeNum
      break;
    }
    default:
      break;
  } 
}

// Function for keypad buttons during recipe confirmation state
void confirmationStateButtonInputs(char key){ 
  switch (key) {
    // button B - confirm selection
    case 'B': {
      Serial.println ("Confirm selection...");
      currentState = WEIGHING;            // switch to weighing state
      break;
    }

    // button C - cancel selection
    case 'C': {
      Serial.println ("Cancel selection...");
      // reset selectedRecipeNum
      selectedRecipeNum = "";      
      currentState = RECIPE_SELECTION;   // go back to recipe selection state 
      break;
    }

    default:
      break;
  } 
}

void weighingStateButtonInputs(char key){ 
  Serial.println (key);
  switch (key) {
    // button A - TARING
    case 'A': { 
      lcd.clear();
      led_colour(0,0,255); //blue light to inform users that item is tared 
      Serial.println ("Taring......");
      lcd.setCursor(0, 0);   // set the cursor to column 0, line 0 
      lcd.print("Taring......");
      scale.tare(); 
      delay(150);//taring timing
      led_colour(255,50,0);
      lcd.clear();//clear screen for taring completed 
      break;
    }

    // button # - go next ingredient
    case '#': {
      Serial.println("Going next ingredient...");
      Serial.println(canGetNextIngredient);
      // if measurement is correct, go to next ingredient
      if (canGetNextIngredient == true) {
        // if there's no more ingredients, weighing completed and return to recipe selection
        if (desiredRecipe.isLastIngredient()){
          lcd.clear();
          lcd.setCursor(0, 0);
          Serial.println ("Weighing Complete!");
          lcd.print("Completed!");
          delay(500);
          currentState = RECIPE_SELECTION;
        } else {
          // if there's still ingredients left, go to next ingredient 
          desiredRecipe.incrementIngredientIndex();
          canGetNextIngredient = false;
        }
      }
      break;
    }
    /* 
       button * - go back to home/recipe selection state
       reset selectedRecipeNum and go back to recipe selection state
       resets the lcd & led as well
    */
    case '*': {
      Serial.println ("Back to home...");
      selectedRecipeNum = "";
      lcd.clear();
      led_colour(0,0,0);
      currentState = RECIPE_SELECTION;
      break;
    }
    default:
      break;
  } 
}

//Function for calculating difference in weights    
int calculateWeightDiff(int weight, int desiredWeight){
  weight = weight - desiredWeight;
  return weight;
}
