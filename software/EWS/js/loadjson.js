window.onload = function () {
    var xhttp;
    if(window.XMLHttpRequest) {
        xhttp = new XMLHttpRequest();
    } else {
        xhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }
    
    var url = window.location.pathname;                         // Get URL
    var filename = url.substring(url.lastIndexOf('/')+1);       // Get HTML page filename
    filename = filename.slice(0,filename.lastIndexOf('.'));     // Remove Extension
    
    xhttp.onreadystatechange = function () {
        var out = "<br><br><table id=\"content\">";
        var col = [];
        var rec = [];
        if(xhttp.readyState == 4 && xhttp.status == 200) {
            //document.getElementById("demo").innerHTML = xhttp.responseText;
            
            //------- PARSE JSON --------------------------
            var obj=JSON.parse(xhttp.responseText);
            if(filename=="ingred"){
                for( i in obj.arduino[0].port ) {
                    out = out + "<th id=\"content\">" + obj.arduino[0].port[i].ingredient + "</th>";
                    col.push(
                        "driver: " + obj.arduino[0].port[i].driver + "<br />" +
                        "pulse: " + obj.arduino[0].port[i].pulse + "<br />" +
                        "<a href=\"#\">Manual On</a>"
                    );
                }
            }
            else if(filename=="recipes"){
                for( i in obj.recipes ) {
                    out = out + "<th id=\"content\" >" + obj.recipes[i].name + "</th>";
                    var ingr = "";
                    // Build Recipe Items
                    for ( j in obj.recipes[i].ingredients ) {
                        ingr = ingr + 
                            obj.recipes[i].ingredients[j].amount + " - " +
                            obj.recipes[i].ingredients[j].unit + " " +
                            obj.recipes[i].ingredients[j].name + "<br>";
                    }
                    
                    // Attach Recipe's Column Details
                    col.push( obj.recipes[i].attribute + "<br />" +
                        "<div " +
                        "onmouseover=\"document.getElementById('recipe" + i + "').style.display = 'inline';\" " +
                        "onmouseout=\"document.getElementById('recipe" + i + "').style.display = 'none';\"><i>Recipe</i></div>" +
                        "<div id=\"recipe" + i + "\" style=\"background-color:yellow;display:none;\">" + ingr + "</div>");
                }
            }
            
        }
        out = out + "<tr>";
        for( r in col ) { out = out + "<td id=\"content\">" + col[r] + "</td>"; }
        document.getElementById("content").innerHTML = out; 
        out = out + "</tr></table>";
        
        // Add recipe details
        if (filename=="recipes") {
            for( var r = 0; r < rec.length; ++r ) {
                if ( document.getElementById("recipe" + r) != null) {
                    document.getElementById("recipe" + r).innerHTML = rec[r];
                }  //Load recipes
            }
        }
        
    };

    xhttp.open("GET", "json/" + filename + ".json", true);      // Load equivalent JSON file
    xhttp.send();
}