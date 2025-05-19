var containsKey = function (json, key) {
    for (json_key in json) {
        if (json_key == key) {
            return true;
        }
    }
    return false;
}

var sendCount = function (count) {
    var dict = { 'Count': count };
    Pebble.sendAppMessage(dict, function () {
        console.log('Message sent successfully: ' + JSON.stringify(dict));
    }, function (e) {
        console.log('Message failed: ' + JSON.stringify(e));
    });
}


Pebble.addEventListener("ready",
    function () {
        console.log("Pebblekit JS ready.");
    }
);

Pebble.addEventListener('appmessage', function (e) {
    console.log('Message received: ' + JSON.stringify(e));

    var count = 0;
    if (containsKey(e.payload, 'Reset')) {
        count = 0;
        console.log('Reset count: ' + count);
        sendCount(count);
    } else if (containsKey(e.payload, 'CountUp')) {
        count = e.payload['CountUp'] + 1;
        console.log('Count up: ' + count);
        sendCount(count);
    }
    else if (containsKey(e.payload, 'CountDown')) {
        count = e.payload['CountDown'] - 1;
        console.log('Count down: ' + count);
        sendCount(count);
    }
});