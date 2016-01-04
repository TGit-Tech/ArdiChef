/*

The MIT License (MIT)

Copyright (c) 2014 Thomas Gittins

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**********************************************************************************************//**
* @file loadjson.js
* @brief Javascript code for the ArdiChef User Interface EWS
*       Code that executes once the 'html' page has finished loading
* @notes
*   SETUP VARIABLES - Variables that allow special functionality for debugging and etc...
*       ARDUINO_IP  Redirects AJAX requests to an outside IP; This can be used to run and edit the
*                   EWS code on a local PC while sending requests to the Arduino.
*                       local computer and make edits while transfering requests to the Arduino.
* @authors
*       tgit23          1/2016      Original
* @todo
*       Consider using G-Code standard
**************************************************************************************************/
// SETUP VARIABLES
var ARDUINO_IP = "";  // Redirect AJAX to IP: example http://192.168.0.25
var DBG = 0;                             // Activate Debug Messages on the EWS page

// CODE VARIABLES
var recipesJSON = null; // JSON object for Recipes
var ingredJSON = null; // JSON object for Ingredients
var Stick = 0;
var changed = 0;
var gparams;
var gurl;

/**********************************************************************************************//**
* @brief ajaxRequest()
* @return AJAX XMLHttpRequest Object
**************************************************************************************************/
function ajaxRequest() {
    if (window.XMLHttpRequest) {
        return new XMLHttpRequest();
    } else {
        return new ActiveXObject("Microsoft.XMLHTTP");
    }
}

/**********************************************************************************************//**
* @brief onload() - anonymous function
*       Code that executes once the 'html' page has finished loading.  This function currently
*       loads the page specific JSON (ie. 'json/<htmlfile>.json') and calls the function to display
*       the JSON items on the page.
**************************************************************************************************/
window.onload = function () {

    // Load the <html>.JSON file
    var url = window.location.pathname;                         // Get URL
    var filename = url.substring(url.lastIndexOf('/')+1);       // Get HTML page filename
    filename = filename.slice(0,filename.lastIndexOf('.'));     // Remove Extension
    
    if( (filename=="ingred" && ingredJSON == null) || (filename=="recipes" && recipesJSON == null) ) {
        var xhttp = ajaxRequest();
        xhttp.open("GET", "json/" + filename + ".json", true);      // Load equivalent JSON file
        xhttp.setRequestHeader("Content-type", "application/json");
        xhttp.setRequestHeader("Connection", "close");
        xhttp.send();
        xhttp.onreadystatechange = function () {
            if(xhttp.readyState == 4 && xhttp.status == 200) {
                if(filename=="ingred") {
                    ingredJSON=JSON.parse(xhttp.responseText);
                    LoadIngredContent();
                } else {
                    recipesJSON=JSON.parse(xhttp.responseText);
                    LoadRecipeContent();
                }
            }
        }
    }
}

/**********************************************************************************************//**
* @brief LoadIngredContent()
*       Loads 'ingred.json' into the web-page "content" element.
*
*
**************************************************************************************************/
function LoadIngredContent() {
    var url = window.location.pathname;                         // Get URL
    var out = "<br><br><table id=\"content\"><tr>";
    var col = [];
    for( i in ingredJSON.arduino[0].port ) {
        if(i%3==0) { out = out + "</tr><tr>"; }
        out = out + "<td id=\"content\"><b><u>" + ingredJSON.arduino[0].port[i].ingredient + "</u></b><br />" +
            "driver: " + ingredJSON.arduino[0].port[i].driver + "<br />" +
            "teaspoon: " + ingredJSON.arduino[0].port[i].teaspoon + "<br />" +
            "<a href=\"" + url + "?nc=" + i + ",5000;\">Manual On</a></td>";
    }
    out = out + "<td id=\"content\"><button onclick=\"AddIngredient()\">Add Ingredient</button></td></tr></table>";
    document.getElementById("content").innerHTML = out; 
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
    for( i in recipesJSON.recipes ) {
        if(i%3==0) { out = out + "</tr><tr>"; }         // Three recipes per row
        out = out + "<td id=\"content\" >";             // Create Master Button for the Recipe
        out = out + "<button onclick=\"RecipeClick(" + i + ")\" id=\"arecipe\">" + 
                    recipesJSON.recipes[i].attribute + " " + 
                    recipesJSON.recipes[i].name + "</button>";
                
        // Build Recipe Details
        var ingr = "";
        for ( j in recipesJSON.recipes[i].ingredients ) {
            ingr = ingr + 
                "<input size=\"2\" " +
                    "value=\"" + recipesJSON.recipes[i].ingredients[j].amount + "\" " +
                    "onChange=\"recipesJSON.recipes["+i+"].ingredients["+j+"].amount = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\"></input> - " +
                "<input size=\"5\" " +
                    "value=\"" + recipesJSON.recipes[i].ingredients[j].unit + "\" " +
                    "onChange=\"recipesJSON.recipes[" + i + "].ingredients[" + j + "].unit = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\"></input> " +
                "<input size=\"10\" " +
                    "value=\"" + recipesJSON.recipes[i].ingredients[j].unit + "\" " +
                    "onChange=\"recipesJSON.recipes[" + i + "].ingredients[" + j + "].unit = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\"></input><br>";
        }
        
        // HTML for Recipe Details and 'Recipe' Text pop-out
        out = out + "<div style=\"width: 100px;cursor:alias;\" " +
            "onmouseover=\"document.getElementById('recipe" + i + "').style.display = 'inline';\" " +
            "onmouseout=\"if(!Stick) {document.getElementById('recipe" + i + "').style.display = 'none';}\" " +
            "onclick=\"if(Stick){Stick=0;}else{Stick=1;}\"><i>details/click-edit</i></div>" +
            "<div id=\"recipe" + i + "\" class=\"recipedetails\" style=\"display:none;\">" + ingr + "</div></td>"
        
    }
    
    // Append an Add Recipe Button
    out = out + "<td id=\"content\" >";
    out = out + "<button onclick=\"AddRecipe()\" id=\"arecipe\">Add Recipe</button></td></tr></table>";
    document.getElementById("content").innerHTML = out;   
    if(DBG) document.getElementById("debug2").innerHTML = JSON.stringify(recipesJSON); 
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
    var jsonstring = JSON.stringify(recipesJSON); //.replace(/ /g,"+");
    var params = jsonstring.match(/[\s\S]{1,79}/g) || [];       // Parse to 84 characters - bigger causes ArdiChef WebServer fragments
    for( i in params ) {
        if(i==0) { params[i] = "fw=" + params[i]; 
        } else { params[i] = "fa=" + params[i]; }
    }
    SendParameters(params,"/json/recipes.json");
    document.getElementById('save').style.backgroundColor='#66FF33';
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
                    steps = MeasureConvert(
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
* @brief SendParameters(params,|url|) - Send a list of parameters to the ArdiChef's Arduino WebServer
* @param params - array of parameters to send
* @param url - optionally provide a custom file location (will default to current file)
**************************************************************************************************/
function SendParameters(params, url) {
    if (params.length==0) return;           // if no more parameters then exit
    
    // Assign a URL if no URL was passed; 
    // Trim the file:://folder// from local host URL's
    if (typeof(url)=="undefined") { 
        url = window.location.href;
        if(ARDUINO_IP.length>0) { url = url.substr(url.lastIndexOf("/")); }
    }
    
    // Append 'ARDUINO_IP' debug redirect if given; 
    // trap for double appending on recursive calls
    if (ARDUINO_IP.length>0) {
        if(url.substr(0,ARDUINO_IP.length) != ARDUINO_IP) { url = ARDUINO_IP + url; }
    }
    
    var param = params.shift();             // Pop 1st parameter on the list
    var sp_xhttp = ajaxRequest();           // Setup a Send Parameters(sp) AJAX Request

    sp_xhttp.onreadystatechange = function() {  // Set the call-back function
        if (sp_xhttp.readyState == 4) {         // && sp_xhttp.status == 200) {.
            SendParameters(params, url);
        }
    }
    sp_xhttp.open("GET", url + "?" + param, true);  // Build the GET request with attached parameter
    sp_xhttp.send();                                // Send the Request
}

/**********************************************************************************************//**
* @brief MeasureConvert(Value, BaseUnit, TargetUnit) - Converts a measurement amount
*
*       3 tsp = tablespoon
*       6 tsp = fl oz
*       48 tsp = cup
*       96 tsp = pint
*       192 tsp = quart
*       768 tsp = gallon
**************************************************************************************************/
function MeasureConvert(Value, BaseUnit, TargetUnit) {
    // Convert Base to teaspoons
    switch(BaseUnit.toLowerCase()) {
        case "teaspoon": Value = Value; break;
        case "tablespoon": Value = Value * 3; break;
        case "fluid oz": Value = Value * 6; break;
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
        case "fluid oz": return Value/6;
        case "cup": return Value/48;
        case "pint": return Value/96;
        case "quart": return Value/192;
        case "gallon": return Value/768;
        default: Value = 0;
    }
}