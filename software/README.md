# Software
#####For the ArdiChef Project

-------------------------------------------------------------------------------------
##Installation (*Hardware Configuration Dependant*)
-------------------------------------------------------------------------------------
####Single Arduino Board with an Ethernet Shield
1. Arduino Setup
    - Load ArdiChefController into Arduino Sketch
    - Set Pre-Compiler Directives
        - IP Adress
        - Gateway
        - Subnet
    - Compile and Upload to the Arduino
    
    - Setup the EWS
        - Copy all files within the EWS folder to the Ethernet Shield SD-Card
        - Plug the card back into the Ethernet Shield


####Arduino Board & Rasberry Pi
*Still in Development - check back later*

-------------------------------------------------------------------------------------
##Architecture
-------------------------------------------------------------------------------------
*Still in Development - check back later*

-------------------------------------------------------------------------------------
#Folders
-------------------------------------------------------------------------------------
| Folder                     | Contents |
| -------------------------- | -------------|
| /ArdiChefController        | Main Control software for Arduino Board(s) |
| /EWS (Embedded Web Server) | The files that make up the web based user interface |
| /WebServerST_Plus          | Arduino Sketch for the Arduino Ethernet shield enabling EWS hosting |