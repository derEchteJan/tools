

let LOOP_INTERVAL = 250;
let LOOP_INTERVAL_DEBUG = 2000; // loop slower in debug mode for better readability of output

let debugCheckbox = document.getElementById("debug_checkbox");
let textInput = document.getElementById("textIn");
let textOutput = document.getElementById("textOut");

function isDebug()
{
    return debugCheckbox.checked == true;
}

function log(string)
{
    if(!isDebug()) return;
    console.log(string);
}

function getLoop()
{
    if(isDebug()) return;
    shmc_get();
}

function getLoopDebug()
{
    if(!isDebug()) return;
    log("debug loop");
    shmc_get();
}

function onLoad()
{
    textOutput.value = ""; 
    textInput.value = "";
    updateInputHeight();
    updateOutputScrollPos();
}

textInput.addEventListener("input", onKeyPressed);

window.setInterval(getLoop, LOOP_INTERVAL);
window.setInterval(getLoopDebug, LOOP_INTERVAL_DEBUG);
window.onload = onLoad();

/**
 * @param {InputEvent} event
 */
function onKeyPressed(event)
{
    if(event.data == null && event.inputType == "insertLineBreak")
    {
        sendPressed();
    }

    updateInputHeight();
}

function updateInputHeight()
{
    textInput.style.height = 0;
    textInput.style.height = textInput.scrollHeight + "px";
}

function updateOutputScrollPos()
{
    textOutput.scrollTop = textOutput.scrollHeight;
}

function sendPressed()
{
    log("sending");
    var inputValue = textInput.value.trim();
    shmc_post(inputValue);
    textInput.value = "";
    updateInputHeight();
}

function getPressed()
{
    log("getting");
    shmc_get();
}

function clearPressed()
{
    textOutput.value = "";
    updateOutputScrollPos();
}

function get_url()
{
    return isDebug() ? "/shmchat.php?debug=true" : "/shmchat.php";
}

/**
 * dispatches get request to load contents of api output buffer
 */
function shmc_get()
{
    const req = new XMLHttpRequest();
    req.open("GET", get_url(), /*async:*/ true);
    req.onload = (event) => {
        if (req.readyState === 4) 
        {
            if (req.status === 200)
                shmc_get_response(req.responseText.trim());
            else
                shmc_get_error(req.statusText);
        }
    };
    req.onerror = (event) => {
        shmc_get_error(req.statusText);
    };
    req.send(null);
}

/**
 * @param {string} response 
 */
function shmc_get_response(response)
{
    log("GET response: \n" + response);

    if(response.length > 0)
    {
        textOutput.value += response + "\n";
        updateOutputScrollPos();
    }
}

/**
 * @param {string} statusText 
 */
function shmc_get_error(statusText)
{
    log("GET error response: \n" + statusText);
}

/**
 * dispatches post request to send command into api input buffer
 * @param {string} text - request body / api command
 */
function shmc_post(text)
{
    const req = new XMLHttpRequest();
    req.open("POST", get_url(), /*async:*/ true);
    req.onload = (event) => {
        if (req.readyState === 4) 
        {
            if (req.status === 200)
                shmc_post_response(req.responseText.trim(), text);
            else
                shmc_post_error(req.statusText);
        }
    };
    req.onerror = (event) => {
        shmc_post_error(req.statusText);
    };
    req.send(text);
}

/**
 * @param {string} response 
 * @param {string} text 
 */
function shmc_post_response(response, text)
{
    log("POST response: \n" + response);

    textOutput.value += text + "\n";
    updateOutputScrollPos();
}

/**
 * @param {string} statusText 
 */
function shmc_post_error(statusText)
{
    log("POST error response: \n" + statusText);
}