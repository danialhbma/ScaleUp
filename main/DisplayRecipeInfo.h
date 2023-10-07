// functions for outputting recipes to serial monitor and LCD

#ifndef DISPLAY_RECIPE_INFO_H
#define DISPLAY_RECIPE_INFO_H

// for debugging - prints all recipe information to serial monitor
void printAllInfoOnSerial(int recipeNo, String recipeName, std::vector<String> ingredients, std::vector<String> quantities) {
  Serial.println("Recipe No.: " + String(recipeNo));
  Serial.println("Recipe Name: " + recipeName);
  for (size_t i = 0; i < ingredients.size(); i++) {
    Serial.println("Ingredient: " + ingredients[i]);
    Serial.println("Quantity: " + quantities[i]);
  }
}

#endif