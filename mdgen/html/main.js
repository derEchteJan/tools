// main.js

var hasPhp = false;

function onLoad()
{
    sendHasPhpRequest();
}

function onEditPressed()
{
    if(hasPhp)
    {
        var path = window.location.pathname;
        window.location.href = "/.actions/edit.html?file=" + path.replace(".html", ".md");
    }
    else
    {
        alert("youre not in edit mode");
    }
}

function onCreatePressed()
{
    if(hasPhp)
    {
        window.location.href = "/.actions/edit.html?file=Neue%20Datei";
    }
    else
    {
        alert("youre not in edit mode");
    }
}

function onWriteModePressed()
{
    window.location = (window.location + "").replace("rezepte.fvsion.de", "rezepte-edit.fvsion.de");
}

function sendHasPhpRequest()
{
    var request = new XMLHttpRequest();
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4 )
        {
            onHasPhpReceived(request.status == 200);
        }
    }
    request.open(/*method:*/ "GET", /*url:*/ "/.actions/has.php?", /*async:*/ true);
    request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
    request.send(null);
}

/**
 * @param {boolean} enabled 
 */
function onHasPhpReceived(enabled)
{
    hasPhp = enabled;
    const writeModeButton = document.getElementById("btn-writemode");
    const editButton = document.getElementById("btn-edit");
    const createButton = document.getElementById("btn-create");
    const spanOn = document.getElementById("edit-on");
    const spanOff = document.getElementById("edit-off");

    if(writeModeButton)
        writeModeButton.disabled = hasPhp;
    if(editButton)
        editButton.disabled = !hasPhp;
    if(createButton)
        createButton.disabled = !hasPhp;

    spanOn.style.display = hasPhp ? "inline" : "none";
    spanOff.style.display = !hasPhp ? "inline" : "none";
}