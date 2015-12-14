var oJSONR; // JSON object for Recipes
var oJSONI; // JSON object for Ingredients
var xhttp;
var filename

window.onload = function () {
    
    // Setup a AJAX request object
    if (xhttp == null) {
        if(window.XMLHttpRequest) {
            xhttp = new XMLHttpRequest();
        } else {
            xhttp = new ActiveXoJSONR.ct("Microsoft.XMLHTTP");
        }
    }
    
    // Load the <html>.JSON file
    var url = window.location.pathname;                         // Get URL
    filename = url.substring(url.lastIndexOf('/')+1);           // Get HTML page filename
    filename = filename.slice(0,filename.lastIndexOf('.'));     // Remove Extension
    xhttp.open("GET", "json/" + filename + ".json", true);      // Load equivalent JSON file
    xhttp.send();
    
    xhttp.onreadystatechange = function () {
        if(xhttp.readyState == 4 && xhttp.status == 200) { 
            oJSONR=JSON.parse(xhttp.responseText);
            if(filename=="ingred") {
                LoadIngredContent();
            } else {
                LoadRecipeContent();
            }
        }
    }
}

function LoadIngredContent() {
    var out = "<br><br><table id=\"content\">";
    var col = [];
    for( i in oJSONR.arduino[0].port ) {
        out = out + "<th id=\"content\">" + oJSONR.arduino[0].port[i].ingredient + "</th>";
        col.push(
            "driver: " + oJSONR.arduino[0].port[i].driver + "<br />" +
            "teaspoon: " + oJSONR.arduino[0].port[i].teaspoon + "<br />" +
            "<a href=\"#\">Manual On</a>"
        );
    }
    out = out + "<tr>";
    for( r in col ) { out = out + "<td id=\"content\">" + col[r] + "</td>"; }
    document.getElementById("content").innerHTML = out; 
    out = out + "</tr></table>";
}

// Function to Load the Recipes in the JSON file
// to the webpage 'content' object
function LoadRecipeContent() {
    
    var col = [];
    var rec = [];
    
    var out = "<br><br><table id=\"content\">";
    for( i in oJSONR.recipes ) {
        
        // Create a Click Button for the Recipe
        out = out + "<td id=\"content\" >";
        out = out + "<button onclick=\"RecipeClick(" + i + ")\" id=\"arecipe\">" + oJSONR.recipes[i].attribute + " " + oJSONR.recipes[i].name + "</button>";
        
        
        // Build Recipe Details
        var ingr = "";
        for ( j in oJSONR.recipes[i].ingredients ) {
            ingr = ingr + 
                oJSONR.recipes[i].ingredients[j].amount + " - " +
                oJSONR.recipes[i].ingredients[j].unit + " " +
                oJSONR.recipes[i].ingredients[j].name + "<br>";
        }
        
        // HTML for Recipe Details and 'Recipe' Text pop-out
        out = out + "<div style=\"width: 10px;cursor:alias;\" " +
            "onmouseover=\"document.getElementById('recipe" + i + "').style.display = 'inline';\" " +
            "onmouseout=\"document.getElementById('recipe" + i + "').style.display = 'none';\"><i>details</i></div>" +
            "<div id=\"recipe" + i + "\" class=\"recipedetails\" style=\"display:none;\">" + ingr + "</div></td>"
    
    }
    out = out + "<tr>";
    
    for( r in col ) { out = out + "<td id=\"content\">" + col[r] + "</td>"; }
    document.getElementById("content").innerHTML = out; 
    out = out + "</tr></table>";
    
    for( var r = 0; r < rec.length; ++r ) {
        if ( document.getElementById("recipe" + r) != null) {
            document.getElementById("recipe" + r).innerHTML = rec[r];
        }  //Load recipes
    }
}

function RecipeClick(i) {
    var url = "";
    var params = "";
    
    // Setup a AJAX request object
    if (xhttp == null) {
        if(window.XMLHttpRequest) {
            xhttp = new XMLHttpRequest();
        } else {
            xhttp = new ActiveXoJSONR.ct("Microsoft.XMLHTTP");
        }
    }

    // Get Ingredients JSON
    if (oJSONI == null ) {
        xhttp.open("GET", "json/ingred.json", true);      // Load Ingredient JSON file
        xhttp.send();
        xhttp.onreadystatechange = function () {
            if(xhttp.readyState == 4 && xhttp.status == 200) { 
                oJSONI=JSON.parse(xhttp.responseText);
            }
        }
    }
    
    // Build the ArdiChef Controller I2C Commands
    for ( j in oJSONR.recipes[i].ingredients ) {        // Iterate recipe ingredients
        for ( k in oJSONI.arduino[0].port ) {           // Iterate available ingredients
            if ( oJSONI.arduino[0].port[k].ingredient.toLowerCase() == oJSONR.recipes[i].ingredients[j].name.toLowerCase() ) {
                url = url + k;      // Record the Port# for the Ingredient
                val = MeasureConvert(
                        oJSONR.recipes[i].ingredients[j].amount,
                        oJSONR.recipes[i].ingredients[j].unit, 
                        "teaspoon") * oJSONI.arduino[0].port[k].teaspoon;
                url = url + "," + val;
            }
        }
        if (val > 0) url = url + ";";
        var val = 0;
    }
  
    // Send the Commands via POST
    xhttp.open("POST", url, true);
    document.getElementById("debug").innerHTML = url;
    
    // Send the proper header information along with the page request
    xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xhttp.setRequestHeader("Content-length", params.length);
    xhttp.setRequestHeader("Connection", "close");
    
    xhttp.send(params);
}

/*
3 tsp = tablespoon
6 tsp = fl oz
48 tsp = cup
96 tsp = pint
192 tsp = quart
768 tsp = gallon
*/
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