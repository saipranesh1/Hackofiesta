var twilio = require('twilio');


// Create a new REST API client to make authenticated requests against the

// twilio back end

var TWILIO_ACCOUNT_SID = '' ;

var TWILIO_AUTH_TOKEN = '';

var OUTGOING_NUMBER = '';

var TWILIO_NUMBER = '';


var client = new twilio.RestClient(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN);


// Pass in parameters to the REST API using an object literal notation. The

// REST client will handle authentication and response serialzation for you.

client.sms.messages.create({

    to:OUTGOING_NUMBER,

    from:TWILIO_NUMBER,

    body:'Hi, sending from my Edison SmartWatch'

}, function(error, message) {

    // The HTTP request to Twilio will run asynchronously. This callback

    // function will be called when a response is received from Twilio

    // The "error" variable will contain error information, if any.

    // If the request was successful, this value will be "falsy"

    if (!error) {

        // The second argument to the callback will contain the information

        // sent back by Twilio for the request. In this case, it is the

        // information about the text messsage you just sent:

        console.log('Success! The SID for this SMS message is:');

        console.log(message.sid);


        console.log('Message sent on:');

        console.log(message.dateCreated);

    } else {

        console.log('error: ' + error.message);

    }

});
