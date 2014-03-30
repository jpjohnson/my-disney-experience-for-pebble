/*********
* MyDisneyExperience for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE WALT DISNEY COMPANY, WALT DISNEY WORLD,
* OR DISNEY PARKS IN ANY WAY
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* pebble-js-app.js
*/

var maxAppMessageBuffer = 100;
var maxAppMessageTries = 3;
var appMessageRetryTimeout = 3000;
var appMessageTimeout = 100;
var httpTimeout = 12000;
var appMessageQueue = [];
var isNewList = false;

var plans = [];

// getItinerary
// ------------
// gets all of the itinerary items for the current data
var getItinerary = function() {
//    console.log("Getting itinerary");

    var today = new Date();
    var dd = today.getDate();
    var mm = today.getMonth() + 1;
    var yyyy = today.getFullYear();

    if (dd < 10) {
        dd = '0' + dd;
    }

    if (mm < 10) {
        mm = '0' + mm;
    }

    var req = new XMLHttpRequest();
    var requestUrl = "http://mde-api.herokuapp.com/date/" + yyyy + "/" + mm + "/" + dd + "/";
    console.log(requestUrl);
    var loginInfo = "username=" + localStorage.username + "&password=" + localStorage.password + "";

    req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    req.setRequestHeader("Content-length", loginInfo.length);
    req.setRequestHeader("Connection", "close");

    req.open('POST', requestUrl, true);
    req.onload = function(e) {
        if (req.readyState == 4) {
            if (req.status == 200) {
                if (req.responseText) {
//                    console.log("Got a response");
                    var response = JSON.parse(req.responseText);
                    plans = [];
                    var destinationsArray = response.content.destinations;
                    if (response.result == 'fail') {
                        sendError("You have no plans for today. Open the app on your phone to add some!");
                    }
                    destinationsArray.forEach(function(element, index, array) {
                        var plansArray = element.plans;
                        plansArray.forEach(function(element, index, array) {
                            var name = element.name;
                            var fastPass = element.fastPass;
                            if (fastPass) {
                                fastPass = 1;
                            } else {
                                fastPass = 0;
                            }
                            var time = element.time;
                            plans.push(element);
                        });
                    });
                    plans.forEach(function(element, index, array) {
                        appMessageQueue.push({'message': {
                            'index': index,
                            'name': element.name,
                            'time': element.time,
                            'fastPass': element.fastPass
                        }});
                    });
                }
            }
        }
        sendAppMessage();
    };
    req.ontimeout = function() {
        sendError("Request timed out!");
    };
    req.onerror = function() {
        sendError("Failed to connect!");
    };
    req.send(loginInfo);
}


// sendError
// ---------
// displays error message on Pebble
var sendError = function(error) {
    Pebble.showSimpleNotificationOnPebble("Error!", error.toString());
}

// sendAppMessage
// --------------
// sends app message queue to pebble
var sendAppMessage = function() {
    if (appMessageQueue.length > 0) {
        var currentAppMessage = appMessageQueue[0];
        currentAppMessage.numTries = currentAppMessage.numTries || 0;
        currentAppMessage.transactionId = currentAppMessage.transactionId || -1;

        if (currentAppMessage.numTries < maxAppMessageTries) {
//            console.log("Sending message");
            Pebble.sendAppMessage(
                currentAppMessage.message,
                function(e) {
                    appMessageQueue.shift();
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageTimeout);
                }, function(e) {
                    console.log("Faled sending AppMessage for transactionId: " + e.data.transactionId + ". Error: " + e.data.error.message);
                    appMessageQueue[0].transactionId = e.data.transactionId;
                    appMessageQueue[0].numTries++;
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageRetryTimeout);
                }
            );
        } else {
            console.log("Faled sending AppMessage after multiple attemps for transactionId: " + currentAppMessage.transactionId + ". Error: None. Here's the message: " + JSON.stringify(currentAppMessage.message));
        }
    }
}

Pebble.addEventListener("ready", function(e) {
    if (localStorage.username && localStorage.password) {
        getItinerary();
    } else {
        sendError("Please login by going to the settings on the Pebble app on your phone.");
    }
});

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL("http://logicalpixels.com/mde/settings.html");
});

Pebble.addEventListener("webviewclosed", function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    localStorage.username = configuration.username.toString();
    localStorage.password = configuration.password.toString();
    if (localStorage.username && localStorage.password) {
        getItinerary();
    } else {
        sendError("You didn't enter your username and password in the settings app.");
    }
})
