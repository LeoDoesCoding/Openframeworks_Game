#include "ofApp.h"
#include <cctype>


//--------------------------------------------------------------
void ofApp::setup() {
    headFont.load("Rianty.ttf", 35);
    msgFont.load("Dongle.ttf", 28);
    bg.load("bgSu.png");
    plant.load("plant1.png");
    userType = false;
    phase();
}


//--------------------------------------------------------------
void ofApp::update() {
    //Dialouge cooldown (1 seconds) prevents skipping through dialouge
    if (wait == 1) {
        if (ofGetElapsedTimeMillis() >= time + 1000) {
            prompt = "Press z";
            wait = 0;
        }
    }
}


//--------------------------------------------------------------
void ofApp::draw() {
    bg.draw(0, 0);
    plant.draw(360, 100);
    //Water and sun level display
    ofSetColor(40, 50, 70);
    ofDrawRectangle(25, 40, 300, 33);
    ofDrawRectangle(25, 90, 300, 33);
    ofSetColor(112, 231, 255);
    ofDrawRectangle(25, 40, (water * 3), 33); //Water level display
    ofSetColor(255, 233, 127);
    ofDrawRectangle(25, 90, (sun * 3), 33); //Sun level display

    //Text
    ofFill();
    ofSetColor(200, 230, 240);
    ofDrawRectRounded(60, 480, 900, 255, 40);
    ofSetColor(110, 170, 200);
    ofSetLineWidth(7);
    ofNoFill();
    ofDrawRectRounded(60, 480, 900, 255, 40);
    ofFill();
    ofSetLineWidth(1);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(dayDis, 460, 40);
    headFont.drawString(json["location"]["name"].asString(), 445, 76);
    ofDrawBitmapString(weather, 20, 20);
    ofSetColor(40, 50, 70);
    msgFont.drawString(msgBox, 250, 550);
    msgFont.drawString(userInp, 345, 600);
    msgFont.drawString(prompt, 430, 680);
    ofSetColor(255, 255, 255);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    //Receives user input
    if (userType) {
        if (key == 8 && userInp.size() > 0) { //Backspace
            userInp = userInp.substr(0, userInp.size() - 1); //Deletes one character
        }
        else if (key == OF_KEY_RETURN) {
            userType = false;
            checkInput();
        }
        else {
            userInp.append(1, (char)key);
        }
    }

    //Go through dialouge
    if (key == 'z' && wait == 0 && !userType) { //If z pressed and user can go forward, initiate cooldown and go to next diag
        wait = 1;
        prompt = "";
        diagState++;
        phase();
        time = ofGetElapsedTimeMillis();
    }
}


//Checks city user enters is a valid city entry in API databse
void ofApp::checkInput() {
    while (userInp.length() == 0) {
        userType = true;
    }

    //Changes input to correct case
    transform(userInp.begin(), userInp.end(), userInp.begin(), [](unsigned char c) { return std::tolower(c); });
    userInp[0] = toupper(userInp[0]);

    //Finds entry in databse
    json.open("https://api.weatherapi.com/v1/current.json?q=" + userInp + "&" + APIKey);
    if (json["location"]["name"].asString().length() == 0) { //If no city of that name found
        msgBox = "     Invalid city. Please enter a valid city.\n          >";
        userType = true;
    }
    else if (isVisited()) { //If already visited, invalid input.
        msgBox = "City already visited. Please enter another city.\n          >";
        userType = true;
    }
    else { //Valid city
        diagState++;
        phase();
    }
}


//Message box + time controler
void ofApp::phase() {
    switch (diagState) {
    case 0:
        msgBox = "                 Welcome to plant sim!\nThe goal is to keep your plant alive for 7 days.";
        wait = 1;
        break;
    case 1:
        msgBox = "    Your plant needs water and light.\n    You'll need to send it to different cities";
        wait = 1;
        break;
    case 2:
        msgBox = "       So, where do you want to go to first?\n          >";
        userType = true;
        wait = 3;
        break;
    case 3:
        msgBox = "                Sending plant to " + userInp + "...";
        userInp = "";
        wait = 1;
        break;
    case 4: { //Checks weather
        dayDis = "Currently in\n\n\n\n\n     Day " + to_string(day);
        if (day == 3) {
            plant.load("plant2.png");
        }
        string forecast = json["current"]["condition"]["text"].asString();
        msgBox = "  It is";
        if (forecast == "Sunny" || forecast == "Clear") {
            bg.load("bgSu.png");
            msgBox += " sunny. Plant soaked up the rays.";
            sun += 40;
            water -= 20;
        }
        else if (forecast == "Partly cloudy") {
            bg.load("bgPC.png");
            msgBox += " partly cloudy. Plant got some sunlight.";
            sun += 0;
            water -= 15;
        }
        else if (forecast == "Overcast" || forecast == "Cloudy") {
            bg.load("bgC.png");
            msgBox += " overcast. No sunlight or water today!";
            sun -= 20;
            water -= 15;
        }
        else if (forecast == "Light rain" || forecast == "Light drizzle" || forecast == "Patchy rain possible" || forecast == "Light freezing rain" || forecast == "Light rain shower" || forecast == "Light sleet" || forecast == "Light sleet showers") {
            bg.load("bgR.png");
            msgBox += " lightly raining. Plant got watered a bit.";
            sun -= 20;
            water += 30;
        }
        else if (forecast == "Moderate rain") {
            bg.load("bgR.png");
            msgBox += " raining. Plant got watered.";
            sun -= 20;
            water += 50;
        }
        else if (forecast == "Fog" || forecast == "Mist") {
            bg.load("bgC.png");
            msgBox += " foggy. Plant got watered a bit.";
            sun -= 20;
            water += 20;
        }
        else if (forecast == "Light snow" || forecast == "Moderate or heavy snow showers" || forecast == "Patchy light snow" || forecast == "Patchy heavy snow" || forecast == "Heavy snow" || forecast == "Patchy snow possible" || forecast == "Light snow showers") {
            bg.load("bgS.png");
            msgBox += " snowing. Plant got watered a bit, but lost alot of sun :(";
            sun -= 40;
            water += 20;
        }
        else {
            msgBox += "ERROR!";
        }
        visited.push_back(userInp);
        //If either sun or water stat exceeds 100, revert to 100
        if (sun > 100) {
            sun = 100;
        }
        else if (sun < 0) {
            sun = 0;
        }
        if (water > 100) {
            water = 100;
        }
        else if (water < 0) {
            water = 0;
        }
        day++;
        wait = 1;
        break;
    }
    case 5: //Check if ran out of sun and/or water
        if (sun == 0) {
            plant.load("plantDead.png");
            msgBox = "O nose! You plant didn't get enough sun and died :(\n                           Game over";
            wait = 3;
        }
        else if (water == 0) {
            plant.load("plantDead.png");
            msgBox = "O nose! Your plant didn't get enough water and died :(\n                           Game over";
            wait = 3;
        }
        else {
            if (day == 8) {
                plant.load("plant3.png");
                msgBox = "You kept your plant alive for 7 days, congrats!\n                           You won!";
                wait = 3;
                break;
            }
            else {
                msgBox = "Where do you want to send your plant to next?\n          >";
                userInp = "";
                userType = true;
                wait = 3;
                diagState = 2; //Loop back
            }
        }
        break;
    }
}


//Checks if user input city has already been visited
bool ofApp::isVisited() {
    for (int i = 0; i < visited.size(); i++) {
        if (visited[i] == userInp) {
            return true;
        }
    }
    visited.push_back(userInp);
    return false;
}