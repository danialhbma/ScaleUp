// recipe class that stores information about each recipe

#ifndef RECIPE_H
#define RECIPE_H

class Recipe {
  public:
    // constructor with default values
    Recipe() : recipeNo(0), recipeName(""), ingredientIndex(0), ingredients(), quantities() {}

    // get recipes from google sheets JSON payload
    void parseFromJSON(const String& jsonString, std::vector<Recipe>& recipes) {
      StaticJsonDocument<4096> doc;
      DeserializationError error = deserializeJson(doc, jsonString);

      // if JSON data is parsed sucessfully, save each recipe
      if (!error) {
          JsonArray valuesArray = doc["values"].as<JsonArray>();
          Recipe currentRecipe;

          for (size_t i = 1; i < valuesArray.size(); i++) {
              JsonArray recipeArray = valuesArray[i].as<JsonArray>();
              
              // check new recipe start condition (if recipe number is defined)
              bool isNewRecipeStart = (recipeArray[0] != "");
              if (isNewRecipeStart || currentRecipe.recipeNo != 0) {
                  // if new recipe, add new Recipe object to vector of recipes
                  if (isNewRecipeStart && currentRecipe.recipeNo != 0) {
                      recipes.push_back(currentRecipe);
                      currentRecipe.clear();
                  }
                  
                  // if new recipe, add new recipe number and recipe name
                  if (isNewRecipeStart) {
                      currentRecipe.recipeNo = recipeArray[0].as<int>();
                      currentRecipe.recipeName = recipeArray[1].as<String>();
                  }
                  
                  // add current recipe's ingredient and quantity
                  currentRecipe.ingredients.push_back(recipeArray[2].as<String>());
                  currentRecipe.quantities.push_back(recipeArray[3].as<float>());
              }
          }

          // Save the last recipe if it's not empty
          if (currentRecipe.recipeNo != 0) {
              recipes.push_back(currentRecipe);
          }
      } else {
          Serial.println("Parsing failed");
      }
    }

    void clear() {
      recipeNo = 0;
      recipeName = "";
      ingredients.clear();
      quantities.clear();
    }

    // returns recipe based on recipe number, throws error if recipe number is not in the list of recipes
    static Recipe getByRecipeNo(int num, std::vector<Recipe>& recipes) {
      for (Recipe& recipe : recipes) {
          if (recipe.recipeNo == num) {
              return recipe;
          }
      }
      throw std::runtime_error("Recipe not found");
    }

    String getRecipeName(){
      return recipeName;
    }

    String getCurrentIngredient() {
      return getIngredientByIndex(ingredientIndex);
    }

    float getQuantityOfCurrentIngredient() {
      return getQuantityByIndex(ingredientIndex);
    }

    // increases ingredient index by 1 to go to next ingredient 
    void incrementIngredientIndex() {
      if (ingredientIndex < ingredients.size()) {
          ingredientIndex++;
      }
    }

    // resets ingredient index back to 0 (1st ingredient)
    void resetIngredientIndex() {
      ingredientIndex = 0;
    }

    // function to indicate if current ingredient is last
    boolean isLastIngredient(){
      if (ingredientIndex == ingredients.size() - 1) {
        return true;
      }
      return false;
    }

  private:
    int recipeNo;
    String recipeName;
    std::vector<String> ingredients; 
    std::vector<float> quantities;
    int ingredientIndex; 

  // returns ingredient at given index in vector
  String getIngredientByIndex(size_t index) const {
    if (index < ingredients.size()) {
      return ingredients[index];
    }
  }

  // returns ingredient weight at given index in vector
  float getQuantityByIndex(size_t index) const {
    if (index < ingredients.size()) {
      return quantities[index];
    }
  }
};

#endif
