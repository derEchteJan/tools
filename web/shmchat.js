

let LOOP_INTERVAL = 250;

let debugCheckbox = document.getElementById("debug_checkbox");
let textInput = document.getElementById("textIn");
let textOutput = document.getElementById("textOut");
let outputScrollView = document.getElementById("outWrap");
let scrollDownButton = document.getElementById("scrollDown");

var followOutput = true;
var blockScrollEvent = false;
var shellHistory = [];
var historyIdx = -1;

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

function onLoad()
{
    textOutput.textContent = ""; 
    textInput.value = "";
    updateInputHeight();
    scrollToBottom();
}

// --- LOOPS AND EVENT LISTENERS ---

textInput.addEventListener("input", onInput);
textInput.addEventListener("keydown", onKeyDown);
outputScrollView.addEventListener("scroll", onOutputScrolled);

window.setInterval(getLoop, LOOP_INTERVAL);
window.onload = onLoad();

/**
 * input text input event handling
 * @param {InputEvent} event
 */
function onInput(event)
{
    if(event.data == null && event.inputType == "insertLineBreak")
    {
        sendPressed();
    }

    updateInputHeight();
}

/**
 * input text key event handling
 * @param {KeyboardEvent} event
 */
function onKeyDown(event)
{
    let ctrl = event.ctrlKey == true;
    let otherMods = event.altKey || event.shiftKey || event.metaKey;
    let noMods = !ctrl && !otherMods;
    if(event.key == "ArrowDown" && noMods) downArrowPressed();
    else if(event.key == "ArrowUp" && noMods) upArrowPressed();
    else if(event.key == "c" && ctrl && !otherMods) cancelPressed();
}

function onOutputScrolled(event)
{
    if(blockScrollEvent)
    {
        blockScrollEvent = false;
        return;
    }
    if(followOutput)
    {
        followOutput = false;
        updateScrollDownButton();
        console.log("followOutput disabled");
    }
}

function updateInputHeight()
{
    return;
    textInput.style.height = 0;
    textInput.style.height = textInput.scrollHeight + "px";
}

function scrollToBottom()
{
    if(!followOutput) return;
    blockScrollEvent = true;
    outputScrollView.scrollTop = outputScrollView.scrollHeight;
    updateScrollDownButton();
}

function updateScrollDownButton()
{
    scrollDownButton.style.display = followOutput ? "none" : "inline";
}

// --- BUTTON ACTIONS ---

function scrollDownPressed()
{
    followOutput = true;
    scrollToBottom();
}

function sendPressed()
{
    log("sending");
    var inputValue = textInput.value.trim();
    shmc_post(inputValue);
    textInput.value = "";
    updateInputHeight();
    shellHistory.push(inputValue);
    historyIdx = -1;
}

function getPressed()
{
    log("getting");
    shmc_get();
}

function clearPressed()
{
    textOutput.textContent = "";
    followOutput = true;
    scrollToBottom();
}

function upArrowPressed()
{
    log("upArrowPressed");
    historyIdx += 1;
    setFromHistory();
}

function downArrowPressed()
{
    log("downArrowPressed");
    historyIdx -= 1;
    setFromHistory();
}

function setFromHistory()
{
    const min = -1;
    const max = shellHistory.length - 1;

    if(historyIdx < min) historyIdx = min;
    if(historyIdx > max) historyIdx = max;

    if(historyIdx == -1)  textInput.value = "";
    else textInput.value = shellHistory[max - historyIdx];
}

function cancelPressed()
{
    log("cancelPressed");
    textInput.value = "";
    historyIdx = -1;
}

// --- API REQUESTS ---

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
    if(isDebug()) textOutput.textContent += "request: GET " + get_url() + "\n";
}

/**
 * @param {string} response 
 */
function shmc_get_response(response)
{
    log("GET response: \n" + response);

    if(response.length > 0)
    {
        textOutput.textContent += response + "\n";
        scrollToBottom();
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
    if(isDebug()) textOutput.textContent += "request: POST " + get_url() + "\n";
}

/**
 * @param {string} response 
 * @param {string} text 
 */
function shmc_post_response(response, text)
{
    log("POST response: \n" + response);
    if(isDebug()) textOutput.textContent += response + "\n";
    else textOutput.textContent += text + "\n";
    scrollToBottom();
}

/**
 * @param {string} statusText 
 */
function shmc_post_error(statusText)
{
    log("POST error response: \n" + statusText);
}

/**
 * dispatches get request to restart server
 */
function sendRestartReq()
{
    const req = new XMLHttpRequest();
    req.open("GET", "/bedrock_restart.php", /*async:*/ true);
    req.onload = (event) => {
        if (req.readyState === 4) 
        {
            if (req.status === 200)
                log("restart request sent");
            else
                log("restart request error: status " + req.status);
        }
    };
    req.onerror = (event) => {
        log("restart request error: event " + event);
    };
    req.send(null);
    textOutput.textContent += "restarting server.." + "\n";
}

// --- DEBUG STUFF ---

function debugFillRandomText() 
{
    var text = "!/bin/bash\n~$ ls -algh /\n";
    text += "\n" + "drwx------   7 root 4.0K Sep 24 20:50 root";
    text += "\n" + "drwxr-xr-x  32 root  760 Oct 13 19:04 run";
    text += "\n" + "lrwxrwxrwx   1 root    8 Sep 16 20:20 sbin -> usr/sbin";
    text += "\n" + "drwxr-xr-x   2 root 4.0K Sep 16 20:20 srv";
    text += "\n" + "dr-xr-xr-x  13 root    0 Oct 13 18:16 sys";
    text += "\n" + "drwxrwxrwt  19 root  860 Oct 13 20:09 tmp";
    text += "\n" + "drwxr-xr-x  12 root 4.0K Sep 16 20:20 usr";
    text += "\n" + "drwxr-xr-x  12 root 4.0K Sep 16 20:38 var";
    text += "\n" + "lrwxrwxrwx   1 root   32 Sep 16 20:20 vmlinuz -> boot/vmlinuz-6.12.43+deb13-amd64";
    text += "\n" + "lrwxrwxrwx   1 root   32 Sep 16 20:20 vmlinuz.old -> boot/vmlinuz-6.12.43+deb13-amd64";
    text += "\n";
    text += "\n" + "~$";
    textOutput.textContent = text;

    shellHistory = ["ls -algh /"];
    historyIdx = -1;
}