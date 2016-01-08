/*

The MIT License (MIT)

Copyright (c) 2014 Thomas Gittins

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**********************************************************************************************//**
* @file recipes.js
* @brief Javascript code for the ArdiChef User Interface EWS 'index.htm' page
* @notes
*   Main variables are kept in 'common.js'
*   Initial Calibrations:
*       75000 = 1/3 Cup Flour = 16 teaspoons so 1 teaspoon = 4687
* @authors
*       tgit23          1/2016      Original
* @todo
*       Consider using G-Code standard
**************************************************************************************************/
var jsonFile = "json/recipes.json";
var RecipesPerRow = 3;                  // How many recipes per row to display

var EditMode = 0;
/**********************************************************************************************//**
* @brief onload() - anonymous function
*       Code that executes once the 'html' page has finished loading.  This function currently
*       loads the page specific JSON (ie. 'json/<htmlfile>.json') and calls the function to display
*       the JSON items on the page.
**************************************************************************************************/
window.onload = function () {
    if( recipesJSON == null ) {
        var xhttp = ajaxRequest();
        xhttp.open("GET", jsonFile, true);      // Load equivalent JSON file
        xhttp.setRequestHeader("Content-type", "application/json");
        xhttp.setRequestHeader("Connection", "close");
        xhttp.send();
        xhttp.onreadystatechange = function () {
            if(xhttp.readyState == 4 && xhttp.status == 200) {
                recipesJSON=JSON.parse(xhttp.responseText);
                LoadRecipeContent();
            }
        }
    }
}

/**********************************************************************************************//**
* @brief LoadRecipeContent()
*   Loads 'recipes.json' into the web-page "content" element.
*   The html created id='save' button color determines when edits are made and not yet saved.
*
*
**************************************************************************************************/
function LoadRecipeContent() {   
    var out = "<br><br><table id=\"content\"><tr>";
    
    // For Each Recipe
    for( r in recipesJSON.recipes ) {
        if(r%RecipesPerRow==0) { out = out + "</tr><tr>"; }
        
        // Master Recipe Button
        out = out + "<td id=\"content\" >";
        out = out + "<button id=\"RecipeButton"+r+"\" class=\"recipe\" onclick=\"RecipeClick(" + r + ")\" id=\"arecipe\">" + 
                    recipesJSON.recipes[r].attribute + " " + 
                    recipesJSON.recipes[r].name + "</button>";
                
        // Build Recipe Details
        var ingr = EditRecipeNameHTML(r);
        for ( i in recipesJSON.recipes[r].ingredients ) { ingr = ingr + IngredientToHTML(r,i); }
        
        // HTML for Recipe Details and 'recipe<r>' pop-out div
        out = out + "<div style=\"width: 100px;cursor:alias;\" " +
            "onmouseover=\"document.getElementById('recipe" + r + "').style.display = 'inline';\" " +
            "onmouseout=\"if(!EditMode) {document.getElementById('recipe" + r + "').style.display = 'none';}\" " +
            "onclick=\"if(EditMode){EditMode=0;}else{EditMode=1;}\"><i>details/click-edit</i></div>" +
            
            "<div id=\"recipe" + r + "\" class=\"recipedetails\" style=\"display:none;\">" + ingr + 
            "</div></td>";
    }
    
    // Append an Add Recipe Button
    if((r-2)%RecipesPerRow==0) { out = out + "</tr><tr>"; }
    out = out + "<td id=\"content\" ><button onclick=\"AddRecipe()\" id=\"arecipe\">Add Recipe</button></td>";
    out = out + "</tr></table>";

    document.getElementById("content").innerHTML = out;   
    if(DBG) document.getElementById("debug2").innerHTML = "<pre>" + JSON.stringify(recipesJSON,null,1) + "</pre>";
}

/**********************************************************************************************//**
* @brief EditRecipeNameHTMLHTML(r)
* @param r The recipe's index number
* @return The html text required to display the ingredient
**************************************************************************************************/
function EditRecipeNameHTML(r) {
    return "" +
        "<input size=\"10\" " +
            "value=\"" + recipesJSON.recipes[r].attribute + "\" " +
            "onChange=\"recipesJSON.recipes["+r+"].attribute = value;" +
                        "document.getElementById('RecipeButton"+r+"').innerHTML = value + ' ' + recipesJSON.recipes["+r+"].name;" +
                        "document.getElementById('save').style.backgroundColor='#FF5050';\" " +
        "></input>" +
        "<input size=\"10\" " +
            "value=\"" + recipesJSON.recipes[r].name + "\" " +
            "onChange=\"recipesJSON.recipes["+r+"].name = value;" +
                        "document.getElementById('RecipeButton"+r+"').innerHTML = recipesJSON.recipes["+r+"].attribute + ' ' + value;" +
                        "document.getElementById('save').style.backgroundColor='#FF5050';\" " + 
        "></input>" +
        "<button onClick=\"DelRecipe("+r+");\" >del</button>" + 
        "<button onClick=\"AddRecipeIngredient('recipe"+r+"',"+r+",-1);\" >+</button><hr>";
}

/**********************************************************************************************//**
* @brief IngredientToHTML(recipe_index, ingredient_index)
* @param r The recipe's index number
* @param i The ingredient's index number
* @return The html text required to display the ingredient
**************************************************************************************************/
function IngredientToHTML(r, i) {
    return "<div id=\"r"+r+"i"+i+"\" >" +
    
    "<input size=\"2\" " +
        "value=\"" + recipesJSON.recipes[r].ingredients[i].amount + "\" " +
        "onChange=\"recipesJSON.recipes["+r+"].ingredients["+i+"].amount = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\" " +
    "></input> - " +
    
    "<select " +
        "onChange=\"recipesJSON.recipes["+r+"].ingredients["+i+"].unit = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\" " +
    ">" + amounts(recipesJSON.recipes[r].ingredients[i].unit) + "</select> " +
    
    "<input size=\"10\" " +
        "value=\"" + recipesJSON.recipes[r].ingredients[i].name + "\" " +
        "onChange=\"recipesJSON.recipes["+r+"].ingredients["+i+"].name = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\" " +
    "></input>" +
    
    "<button onClick=\"DelRecipeIngredient('recipe"+r+"',"+r+","+i+");\" >-</button>" +
    "<button onClick=\"AddRecipeIngredient('recipe"+r+"',"+r+","+i+");\" >+</button>" +
    "<br /></div>";
}

/**********************************************************************************************//**
* @brief AddRecipe()
*       Adds a new recipe item
* @return none
**************************************************************************************************/
function AddRecipe() {
    var obj = {"name":"RECIPE","attribute":"NEW","ingredients":[]};
    recipesJSON['recipes'].push(obj);
    document.getElementById('save').style.backgroundColor='#FF5050';
    LoadRecipeContent();
}

/**********************************************************************************************//**
* @brief DelRecipe(r)
* @param r The index of the recipe to be deleted
* @return none
**************************************************************************************************/
function DelRecipe(r) {
    recipesJSON['recipes'].splice(r,1);
    document.getElementById('save').style.backgroundColor='#FF5050';
    LoadRecipeContent();
}

/**********************************************************************************************//**
* @brief AddRecipeIngredient(div_id, recipe_index)
*       Adds an ingredient to a recipe
* @param div_id The recipe's details 'div' element name/id
* @param r The recipe's index number
* @param i The index of the ingredient to add below
* @return none
**************************************************************************************************/
function AddRecipeIngredient(div_id, r, i) {
    var el = document.getElementById(div_id);
    /*
    if(typeof(r)=='undefined') { 
        recipesJSON['recipes'].push({"name":"","attribute":"","ingredients":[]});
    }*/
    var obj = {"name":"","amount":0,"unit":""};
    recipesJSON['recipes'][r]['ingredients'].splice(i+1,0,obj);
    
    // Build the new Ingredient details
    var ingr = EditRecipeNameHTML(r);
    for ( c in recipesJSON.recipes[r].ingredients ) { ingr = ingr + IngredientToHTML(r,c); }
    el.innerHTML = ingr;
    
    if(DBG) document.getElementById("debug2").innerHTML = "<pre>" + JSON.stringify(recipesJSON,null,1) + "</pre>";
}

/**********************************************************************************************//**
* @brief DelRecipeIngredient(div_id, recipe_index)
*       Deletes an ingredient that is part of a recipe
* @param div_id The recipe's details 'div' element name/id
* @param r The recipe's index number
* @param i The index of the ingredient to delete
* @return none
**************************************************************************************************/
function DelRecipeIngredient(div_id, r, i) {
    var el = document.getElementById(div_id);
    delete recipesJSON['recipes'][r]['ingredients'].splice(i,1);
    
    // Build the new Ingredient details
    var ingr = EditRecipeNameHTML(r);
    for ( c in recipesJSON.recipes[r].ingredients ) { ingr = ingr + IngredientToHTML(r,c); }
    el.innerHTML = ingr;
    
    if(DBG) document.getElementById("debug2").innerHTML = "<pre>" + JSON.stringify(recipesJSON,null,1) + "</pre>";
}
/**********************************************************************************************//**
* @brief SaveChanges() - Save changes made to recipes using the JSON object
*       Builds and sends the recipesJSON.stringify() to the Arduino to be saved using (fw)(fa) tags.
* @notes
*       The ArdiChef's Arduino WebServer can only handles 254-character requests.  Parsed as
*       |7-character Method|1-space|237-character Request|1-space|8-character Protocol| = 254 total.
*       When special characters like spaces and quotes are 'urlencoded()' they expand to 3-character
*       sizes therefore the max line length to avoid bursting over the limit on 'urlencode()' is
*       237/3 = 79.
*
**************************************************************************************************/
function SaveChanges() {
    if(document.getElementById("save").style.backgroundColor=="rgb(102, 255, 51)") { return; }
    SaveJSON(recipesJSON, jsonFile);
    document.getElementById('save').style.backgroundColor='#66FF33';
    if(DBG) document.getElementById("debug2").innerHTML = "<pre>" + JSON.stringify(recipesJSON,null,1) + "</pre>";
}
/**********************************************************************************************//**
* @brief RecipeClick(i)
*       Builds and sends the Numerical Control (NC) Codes to Arduino via AJAX when recipe is clicked.
*
*
**************************************************************************************************/
function RecipeClick(i) {
    var params = "";
    var db_out = "";
    
    // Clear the debug element
    document.getElementById("debug").innerHTML = "";

    // Get Ingredients JSON
    if (ingredJSON == null ) {
        var xhttp = ajaxRequest();
        xhttp.open("GET", "json/ingred.json", true);      // Load Ingredient JSON file
        xhttp.send();
        xhttp.onreadystatechange = function () {
            if(xhttp.readyState == 4 && xhttp.status == 200) { 
                ingredJSON=JSON.parse(xhttp.responseText);
                RecipeClick(i);
            }
        }
    }
    
    if (ingredJSON != null) {
        // Build the NC-Codes (ie. ArdiChef Controller I2C Commands)
        var nc_codes = [];
        for ( j in recipesJSON.recipes[i].ingredients ) {        // Iterate the ingredients in the recipe
            var nc = ""; var steps = 0;                     // Initialize variables
            for ( k in ingredJSON.arduino[0].port ) {           // Iterate available ingredients
                if ( ingredJSON.arduino[0].port[k].ingredient.toLowerCase() == recipesJSON.recipes[i].ingredients[j].name.toLowerCase() ) {
                    steps = AmountConvert(
                                recipesJSON.recipes[i].ingredients[j].amount,
                                recipesJSON.recipes[i].ingredients[j].unit, 
                                "teaspoon") * ingredJSON.arduino[0].port[k].teaspoon;   //Change Amount -> Steps
                    nc = "nc=" + k + "," + steps + ";";
                    break;                                  // Break the loop when ingredient is found
                }
            }
            if (steps > 0) { nc_codes.push(nc); }
        }
        
        if(DBG) {
            var db_out = "";
            for( var r = 0; r < nc_codes.length; ++r ) db_out = db_out + "<br>" + nc_codes[r];
            document.getElementById("debug").innerHTML = db_out;    // Display on screen for debug
        }

        SendParameters(nc_codes);                               // Send the NC-Codes to the Arduino
    }
}

/**********************************************************************************************//**
* @brief AmountConvert(Value, BaseUnit, TargetUnit) - Converts a measurement amount
*
*       3 tsp = tablespoon
*       6 tsp = fl oz
*       48 tsp = cup
*       96 tsp = pint
*       192 tsp = quart
*       768 tsp = gallon
**************************************************************************************************/
function AmountConvert(Value, BaseUnit, TargetUnit) {
    // Convert Base to teaspoons
    switch(BaseUnit.toLowerCase()) {
        case "teaspoon": Value = Value; break;
        case "tablespoon": Value = Value * 3; break;
        case "fluidoz": Value = Value * 6; break;
        case "cup": Value = Value * 48; break;
        case "pint": Value = Value * 96; break;
        case "quart": Value = Value * 192; break;
        case "gallon": Value = Value * 768; break;
        default: Value = 0;
    }
    // Convert teaspoons to TargetUnit
    switch(TargetUnit.toLowerCase()) {
        case "teaspoon": return Value;
        case "tablespoon": return Value/3;
        case "fluidoz": return Value/6;
        case "cup": return Value/48;
        case "pint": return Value/96;
        case "quart": return Value/192;
        case "gallon": return Value/768;
        default: Value = 0;
    }
}

/**********************************************************************************************//**
* @brief amounts(selected)
* @param selected - The selected item in the 'select' options
* @return The html 'option' portion of a 'select' element with standard measurements
**************************************************************************************************/
function amounts(selected) {
    return "<option value=\"teaspoon\" " + ((selected=="teaspoon") ? "selected=\"selected\"" : "") + ">teaspoon</option>" +
            "<option value=\"tablespoon\" " + ((selected=="tablespoon") ? "selected=\"selected\"" : "") + ">tablespoon</option>" +
            "<option value=\"fluidoz\" " + ((selected=="fluidoz") ? "selected=\"selected\"" : "") + ">fluidoz</option>" +
            "<option value=\"cup\" " + ((selected=="cup") ? "selected=\"selected\"" : "") + ">cup</option>" +
            "<option value=\"pint\" " + ((selected=="pint") ? "selected=\"selected\"" : "") + ">pint</option>" +
            "<option value=\"quart\" " + ((selected=="quart") ? "selected=\"selected\"" : "") + ">quart</option>" +
            "<option value=\"gallon\" " + ((selected=="gallon") ? "selected=\"selected\"" : "") + ">gallon</option>"
}