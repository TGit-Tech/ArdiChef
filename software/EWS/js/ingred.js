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
* @file ingred.js
* @brief Javascript code for the ArdiChef User Interface EWS 'ingred.htm' page
* @notes
*   Main variables are kept in 'common.js'
*   Initial Calibrations:
*       75000 = 1/3 Cup Flour = 16 teaspoons so 1 teaspoon = 4687
* @authors
*       tgit23          1/2016      Original
* @todo
*       Consider using G-Code standard
**************************************************************************************************/
var jsonFile = "json/ingred.json";
var IngredientsPerRow = 3;                  // How many recipes per row to display

/**********************************************************************************************//**
* @brief onload() - anonymous function
*       Code that executes once the 'html' page has finished loading.  This function currently
*       loads the page specific JSON (ie. 'json/<htmlfile>.json') and calls the function to display
*       the JSON items on the page.
**************************************************************************************************/
window.onload = function () {
    if( ingredJSON == null ) {
        var xhttp = ajaxRequest();
        xhttp.open("GET", jsonFile, true);      // Load equivalent JSON file
        xhttp.setRequestHeader("Content-type", "application/json");
        xhttp.setRequestHeader("Connection", "close");
        xhttp.send();
        xhttp.onreadystatechange = function () {
            if(xhttp.readyState == 4 && xhttp.status == 200) {
                    ingredJSON=JSON.parse(xhttp.responseText);
                    LoadIngredContent();
            }
        }
    }
}

/**********************************************************************************************//**
* @brief Drivers(selected)
* @param selected - The selected item in the 'select' options
* @return The html 'option' portion of a 'select' element with supported drivers
**************************************************************************************************/
function Drivers(selected) {
    return "" +
        "<option value=\"ULN2003\" " + ((selected=="ULN2003") ? "selected=\"selected\"" : "") + ">ULN2003</option>" +
        "<option value=\"A4988\" " + ((selected=="A4988") ? "selected=\"selected\"" : "") + ">A4988</option>";
}

/**********************************************************************************************//**
* @brief DriverHTML(selected)
* @param a Arduino index number
* @param p Port number
* @param driver The driver to be displayed as HTML
* @return The html required to show all parameters of the driver
**************************************************************************************************/
function DriverHTML(a,p,driver) {
    var ret = "";
    if ( driver == "ULN2003" ) {
        ret = "<button onClick=\"manual("+p+",document.getElementById('steps"+p+"').value);\">On</button>" +
            "<button onClick=\"manual("+p+",5)\">Off</button> Manual " + 
            "<input id=\"steps"+p+"\" size=\"5\" value=\"5000\"></input><br />" +
            "One Teaspoon = <input size=\"5\" " +
                "value=\"" + ingredJSON.arduino[a].port[p].teaspoon + "\" " +
                "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].teaspoon = value;" +
                "document.getElementById('save').style.backgroundColor='#FF5050';\"></input> steps<br />" +
            "MotorPins: <input size=\"10\" " +
                "value=\"" + ingredJSON.arduino[a].port[p].motorpins + "\" " +
                "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].motorpins = value;" +
                "document.getElementById('save').style.backgroundColor='#FF5050';\"></input><br />";
    } else if ( driver == "A4988" ) {
        ret = "<button onClick=\"manual("+p+",document.getElementById('steps"+p+"').value);\">On</button>" +
            "<button onClick=\"manual("+p+",5)\">Off</button> Manual " + 
            "<input id=\"steps"+p+"\" size=\"5\" value=\"5000\"></input><br />" +
            "One Teaspoon = <input size=\"5\" " +
                "value=\"" + ingredJSON.arduino[a].port[p].teaspoon + "\" " +
                "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].teaspoon = value;" +
                "document.getElementById('save').style.backgroundColor='#FF5050';\"></input> steps<br />" +
            "StepPin: <input size=\"2\" " +
                "value=\"" + ingredJSON.arduino[a].port[p].steppin + "\" " +
                "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].steppin = value;" +
                "document.getElementById('save').style.backgroundColor='#FF5050';\"></input><br />" +
            "DirPin: <input size=\"2\" " +
                "value=\"" + ingredJSON.arduino[a].port[p].dirpin + "\" " +
                "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].dirpin = value;" +
                "document.getElementById('save').style.backgroundColor='#FF5050';\"></input><br />";
    }
    return ret;
}

/**********************************************************************************************//**
* @brief LoadIngredContent()
*       Loads 'ingred.json' into the web-page "content" element.
*
*
**************************************************************************************************/
function LoadIngredContent() {
    var out = "";
    
    // For each arduino board
    for( a in ingredJSON.arduino ) {
        out = out + "<b>Arduino: " + a + "</b>&nbsp;&nbsp;&nbsp;" +
            "I2C(" + ingredJSON.arduino[a].i2c + ")" +
            "<table id=\"content\"><tr>";
        
        // For each port
        for( p in ingredJSON.arduino[a].port ) {
            if(p%IngredientsPerRow==0) out = out + "</tr><tr>";
                        
            out = out + 
            "<td id='content'>" +
                "<center><input size=\"10\" " +
                    "value=\"" + ingredJSON.arduino[a].port[p].ingredient + "\" " +
                    "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].ingredient = value;" +
                    "document.getElementById('save').style.backgroundColor='#FF5050';\" " +
                "></input></center><hr />" +
                
                "A"+a+":P"+p+":Driver " + 
                "<select " +
                    "onChange=\"ingredJSON.arduino["+a+"].port["+p+"].driver = value;" +
                        "document.getElementById('DriverA"+a+"P"+p+"').innerHTML = DriverHTML("+a+","+p+",value);" +
                        "document.getElementById('save').style.backgroundColor='#FF5050';\" " +
                    ">" + Drivers(ingredJSON.arduino[a].port[p].driver) +
                "</select><hr /> ";
                
            // Display Driver Properties
            out = out + "<div id='DriverA"+a+"P"+p+"' >" + DriverHTML(a,p,ingredJSON.arduino[a].port[p].driver) + "</div>"; 

            out = out + "<button onClick=\"DelPort("+a+","+p+");\">delete port</button>";
            out = out + "</td>";
            
        }
        if((p-2)%IngredientsPerRow==0) out = out + "</tr><tr>";
        out = out + "<td id=\"content\"><center><button onclick=\"AddIngredient("+a+")\">Add Ingredient</button></center></td>";
        out = out + "</tr></table>";
    }
    
    document.getElementById("content").innerHTML = out; 
}

function manual(port,steps) {
    var nc_codes = [];
    nc_codes.push("nc="+port+","+steps+";");
    SendParameters(nc_codes);
}

function AddIngredient(a) {
    var obj = {"ingredient":"NEW","driver":"NEW"};
    ingredJSON['arduino'][a]['port'].push(obj);
    document.getElementById('save').style.backgroundColor='#FF5050';
    LoadIngredContent();
}
/**********************************************************************************************//**
* @brief DelRecipe(r)
* @param r The index of the recipe to be deleted
* @return none
**************************************************************************************************/
function DelPort(a,p) {
    ingredJSON['arduino'][a]['port'].splice(p,1);
    document.getElementById('save').style.backgroundColor='#FF5050';
    LoadIngredContent();
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
    SaveJSON(ingredJSON, jsonFile);
    document.getElementById('save').style.backgroundColor='#66FF33';
    if(DBG) document.getElementById("debug2").innerHTML = "<pre>" + JSON.stringify(recipesJSON,null,1) + "</pre>";
}