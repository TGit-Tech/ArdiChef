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
* @file loadjson.js
* @brief Javascript code for the ArdiChef User Interface EWS
*       Code that executes once the 'html' page has finished loading
* @notes
*   SETUP VARIABLES - Variables that allow special functionality for debugging and etc...
*       ARDUINO_IP  Redirects AJAX requests to an outside IP; This can be used to run and edit the
*                   EWS code on a local PC while sending requests to the Arduino.
*                       local computer and make edits while transfering requests to the Arduino.
*   Initial Calibrations:
*       75000 = 1/3 Cup Flour = 16 teaspoons so 1 teaspoon = 4687
* @authors
*       tgit23          1/2016      Original
* @todo
*       Consider using G-Code standard
**************************************************************************************************/
// SETUP VARIABLES
var ARDUINO_IP = "http://192.168.0.25";  // Redirect AJAX to IP: example http://192.168.0.25
var DBG = 0;                             // Activate Debug Messages on the EWS page

// CODE VARIABLES
var recipesJSON = null; // JSON object for Recipes
var ingredJSON = null; // JSON object for Ingredients
var thispage = "";

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

window.onload = function () {
    // Load the <html>.JSON file
    var url = window.location.pathname;                         // Get URL
    thispage = url.substring(url.lastIndexOf('/')+1);           // Get HTML page filename
    thispage = thispage.slice(0,thispage.lastIndexOf('.'));     // Remove Extension
}

/**********************************************************************************************//**
* @brief SaveJSON() - Save changes made to recipes using the JSON object
*       Builds and sends the recipesJSON.stringify() to the Arduino to be saved using (fw)(fa) tags.
* @notes
*       The ArdiChef's Arduino WebServer can only handles 254-character requests.  Parsed as
*       |7-character Method|1-space|237-character Request|1-space|8-character Protocol| = 254 total.
*       When special characters like spaces and quotes are 'urlencoded()' they expand to 3-character
*       sizes therefore the max line length to avoid bursting over the limit on 'urlencode()' is
*       237/3 = 79.
*
**************************************************************************************************/
function SaveJSON(oJSON, fileName) {
    var jsonstring = JSON.stringify(oJSON);
    
    // Parse to 84 characters - bigger causes ArdiChef WebServer fragments
    var params = jsonstring.match(/[\s\S]{1,79}/g) || [];       
    for( i in params ) {
        if(i==0) { params[i] = "fw=" + params[i]; 
        } else { params[i] = "fa=" + params[i]; }
    }
    SendParameters(params,fileName);
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
        if(url.substr(0,ARDUINO_IP.length) != ARDUINO_IP) { url = ARDUINO_IP + "/" + url; }
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