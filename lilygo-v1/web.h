const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>Project 999</title>
    <style>
        * {
            margin: 0;
        }

        html {
            font-family: Helvetica;
        }

        body {
            background-color: #000;
        }

        h1 {
            color: #D6E32C;
        }

        p {
            font-size: 16.2dvw;
            color: #D6E32C;
        }

        .textContainer {
            margin: auto;
            justify-content: center;
            position: fixed;
            inset: 0px;
            width: 80rem;
            height: 80rem;
            max-width: 90dvw;
            max-height: 90dvh;
            display: block;
            align-items: center;
            text-align: center;
        }

        .switch {
            position: absolute;
            left: 0;
            top: 0;
            width: 80px;
            margin-right: 0px;
        }

        .toggle {
            display: none
        }

        .toggle+label {
            display: block;
            position: relative;
            cursor: pointer;
            outline: 0;
            user-select: none;
            padding: 2px;
            width: 40px;
            height: 20px;
            background-color: #ddd;
            border-radius: 40px
        }

        .toggle+label:before,
        .toggle+label:after {
            display: block;
            position: absolute;
            top: 1px;
            left: 1px;
            bottom: 1px;
            content: ""
        }

        .toggle+label:before {
            right: 1px;
            background-color: #f1f1f1;
            border-radius: 20px;
            transition: background .4s
        }

        .toggle+label:after {
            width: 40px;
            background-color: #fff;
            border-radius: 20px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, .3);
            transition: margin .4s
        }

        .toggle:checked+label:before {
            background-color: #4285f4
        }

        .toggle:checked+label:after {
            margin-left: 42px
        }
    </style>
</head>

<body>


    <div class="switch">
        <input id="toggle-FS" class="toggle" type="checkbox" openFullscreen>
        <label for="toggle-FS"></label>
    </div>

    <div class="textContainer">
        <p class="overflow-wrap overflow-wrap--anywhere" id="msgReceived"> %STATE%</p>
    </div>



    <script>
        window.addEventListener('load', function () {
            function connect() {

                var websocket = new WebSocket(`ws://${window.location.hostname}/ws`);
                websocket.onopen = function (event) {
                    console.log('Connection established');
                }
                websocket.onclose = function (event) {
                    console.log('Connection died. Reconnect will be attempted in 1 second.', event.reason);
                    setTimeout(function () {
                        connect();
                        document.getElementById('msgReceived2').innerHTML = "...";
                        //location.reload(true);
                    }, 1000);
                }
                websocket.onerror = function (error) {
                    console.log('error');
                    websocket.close();
                }
                websocket.onmessage = function (event) {
                    if (event.data == "1") {
                        document.getElementById('msgReceived').innerHTML = "";
                        //document.getElementById('toggle-btn').checked = true;
                    }
                    else if (event.data == "ALIVE") {
                    } else {
                        //document.getElementById('toggle-btn').checked = false;
                        document.getElementById('msgReceived').innerHTML = event.data;
                        // setTimeout(function () {
                        //     connect();
                        //     document.getElementById('msgReceived').innerHTML = "restarting...";
                        //     location.reload(true);
                        // }, 65000);
                    }
                    console.log('Message:', event.data);
                }
                //add interval for keepAlive
                const ping_interval = 30000; // it's in milliseconds, which equals to 30 seconds
                let interval;
                websocket.addEventListener('open', (event) => {
                    console.log('websocket connection established: ', event);
                    const sendMessage = JSON.stringify({ ping: 1 });
                    websocket.send(sendMessage);

                    // to Keep the connection alive
                    interval = setInterval(() => {
                        const sendMessage = JSON.stringify({ ping: 1 });
                        websocket.send(sendMessage);
                    }, ping_interval);
                });

                // subscribe to `close` event
                websocket.addEventListener('close', (event) => {
                    console.log('websocket connectioned closed: ', event);
                    clearInterval(interval);
                });
                // document.getElementById('toggle-btn').addEventListener('change', function () {
                //     websocket.send('toggle');
                // });

            }

            connect();


        });

        /* When the openFullscreen() function is executed, open the video in fullscreen.
        Note that we must include prefixes for different browsers, as they don't support the requestFullscreen property yet */
        var elem = document.documentElement;
        function openFullscreen() {
            if (elem.requestFullscreen) {
                elem.requestFullscreen();
            } else if (elem.webkitRequestFullscreen) { /* Safari */
                elem.webkitRequestFullscreen();
            } else if (elem.msRequestFullscreen) { /* IE11 */
                elem.msRequestFullscreen();
            }
        }

        //();

        document.getElementById('toggle-FS').addEventListener('change', function () {
            openFullscreen();
        });

        // document.querySelectorAll('button').forEach($button =>
        //     $button.onclick = () => document.querySelector('dialog').removeAttribute('open'))





    </script>
</body>

</html>
)rawliteral";