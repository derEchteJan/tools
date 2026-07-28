// edit.js

var s_pathInput = document.getElementById("ta_file_path");
var s_textArea = document.getElementById("ta_text_input");
var s_overlay = document.getElementById("overlay");

const s_imageFormats = [ "png", "jpeg", "jpg", "webp", "gif", "bmp" ];
const s_attachmentsDir = ".attachments";

function onLoad()
{
    SetFilePathFromUlr();

    onFetchPressed();

    var overlay = document.getElementById("overlay");
    closeOverlayOnClick(overlay);
    var overlay_main = document.getElementById("overlay-main");
    closeOverlayOnClick(overlay_main);
};

function getFilePath(skipLastComponent)
{
    var filePath = '';
    var parts = s_pathInput.value.trim().split('/');
    if(parts.length == 0)
    {
        filePath = '/';
    }
    else
    {
        for(var i = 0; i < parts.length; i++)
        {   
            var part = parts[i];
            if(!part) continue;
            if(i == parts.length - 1 && skipLastComponent === true) continue;
            part = part.trim();
            part = part.replaceAll(" ", "_");
            filePath += "/" + part;
        }
    }
    //console.log("pre filter: " + filePath);
    var regex = /[A-Za-z0-9\.\/_-]+/g;
    filePath = (filePath.match(regex) || []).join('');
    //console.log("post filter: " + filePath);
    if(!filePath.endsWith('.md') && !skipLastComponent)
    {
        filePath += '.md';
    }
    return filePath;
}

function SetFilePathFromUlr()
{
    const params = new URLSearchParams(window.location.search);
    var result = "";
    for (const [key, value] of params)
    {
        if(key == "file") result = value;
    }
    s_pathInput.value = result;
}

function savePressed()
{
    var content = s_textArea.value;
    var filePath = getFilePath();
    postDocument(filePath, content, true);
}

function reloadPressed()
{
    onFetchPressed();
}

function cancelPressed()
{
    window.history.go(-1);
}

function onPostPressed()
{
    var content = s_textArea.value;
    var filePath = getFilePath();
    postDocument(filePath, content);
}

function onFetchPressed()
{
    var filePath = getFilePath();
    fetchDocument(filePath);
}

function setOverlayVisible(visible)
{
    s_overlay.style.display = visible === true ? "inline" : "none";
    if(visible === true)
        loadAttachments();
}

/**
 * @param {string} path 
 * @param {string} content 
 */
function postDocument(path, content, redirect)
{
    var request = new XMLHttpRequest();
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4)
        {
            console.log("status: " + request.status + "\ncontent: \n" + request.responseText);
            if(request.status == 200)
                onDocumentUploaded(redirect);
            else
                console.log("upload error"); // todo handling
        }
    }
    console.log("POST /.actions/edit.php?file=" + encodeURIComponent(path))
    request.open(/*method:*/ "POST", /*url:*/ "/.actions/edit.php?file=" + encodeURIComponent(path), /*async:*/ true);
    request.send(content);
}

function fetchDocument(path, redirect)
{
    s_textArea.placeholder = "loading file..."

    var request = new XMLHttpRequest();
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4 )
        {
            if(request.status == 200)
            {
                onDocumentReceived(request.responseText, redirect);
            }
            else
            {
                onDocumentFetchFailed(request.responseText);
            }
        }
    }
    request.open(/*method:*/ "GET", /*url:*/ path, /*async:*/ true);
    request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
    request.send(null);
}

/**
 * @param {string} content 
 */
function onDocumentReceived(content)
{
    s_textArea.value = content;
    s_textArea.placeholder = "";
}

/**
 * @param {string} content 
 */
function onDocumentFetchFailed(content)
{
    s_textArea.value = "";
    s_textArea.placeholder = "empty file";
}

/**
 * @param {boolean} redirect 
 */
function onDocumentUploaded(redirect)
{
    var filepath = getFilePath().replace(".md", ".html");

    var resultLink = document.getElementById("link_result");
    resultLink.href = filepath;
    resultLink.textContent = filepath;

    if(redirect === true)
    {
        window.location = filepath.replace(".md", ".html") + "?reload=" + Math.floor(Math.random() * 10000); // random url param to force reload when navigating back to original page
    }
}

/* ----------------------------------------- */
/* Attachment Upload Overlay                 */
/* ----------------------------------------- */

function submitForm()
{
    postFormRequest(onPostAttachmentReceived);
}

function onPostAttachmentReceived(success, responseText)
{
    if(success === true)
    {
        alert("document uploaded");
        loadAttachments();
        clearForm();
    }
    else
    {
        alert("document upload error: " + responseText);
    }
}

function postFormRequest(onReceived)
{
    var form = document.getElementById("upload-form");
    var formData = new FormData(form);
    var dir = getFilePath(true);

    var request = new XMLHttpRequest();
    var dir = getFilePath(true);
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4 )
        {
            if(request.status == 200)
            {
                onReceived(true, request.responseText);
            }
            else
            {
                onReceived(false, request.responseText);
            }
        }
    }
    request.open("POST", "/.actions/upload.php?dir=" + encodeURIComponent(dir)); 
    request.send(formData);
}

function clearForm()
{
    var form = document.getElementById("upload-form");
    var inputs = form.elements[0];
    for(var i = 0; i < inputs.length; i++)
    {
        var input = inputs[i];
        input.value = "";
    }
}

function fetch2(path, onReceived)
{
    s_textArea.placeholder = "loading file..."

    var request = new XMLHttpRequest();
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4 )
        {
            if(request.status == 200)
            {
                onReceived(true, request.responseText);
            }
            else
            {
                onReceived(false, request.responseText);
            }
        }
    }
    request.open(/*method:*/ "GET", /*url:*/ path, /*async:*/ true);
    request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
    request.send(null);
}

function loadAttachments()
{
    var dir = getFilePath(/*skipLastComponent:*/ true) + "/" + s_attachmentsDir;
    fetch2("/.actions/ls.php?dir=" + encodeURIComponent(dir), onLoadAttachmentsReceived);
}

function onLoadAttachmentsReceived(success, responseText)
{
    if(success === true)
    {
        parseAttachmentList(responseText);
    }
    else
    {
        console.log("loading attachemnts failed, response: '" + responseText + "'");
    }
}

function parseAttachmentList(rawValue)
{
    var list = rawValue.split(";");

    var placeholder = document.getElementById("no-attachments");
    var table = document.getElementById("attachment-table");
    clearTable(table);
    
    var basePath = getFilePath(true);

    var itemsAdded = 0;
    for(var i = 0; i < list.length; i++)
    {
        var item = list[i].trim();
        if(item.length > 0)
        {
            var sourceLink = "<a href=\"" + basePath + "/" + s_attachmentsDir + "/" + item + "\">source</a>";
            var isImage = false;
            var parts = item.split(".");
            if(parts.length > 0)
            {
                var suffix = parts[parts.length - 1];
                for(var type of s_imageFormats)
                    if(type == suffix)
                        isImage = true;
            }
                
            var clipboardText = (isImage ? "![" : "[") + item + "](" + (isImage ? "" : (s_attachmentsDir + "/")) + item + ")";
            var copyButton = "<button onclick=\"toClipboard('" + clipboardText + "')\">Copy code</button>";
            insertRow(table, [item, sourceLink, copyButton]);
            itemsAdded++;
        }
    }

    // show/hide placeholder vs table depending on list empty or not
    placeholder.style.display = itemsAdded > 0 ? "none" : "inline";
    table.style.display = itemsAdded > 0 ? "inline" : "none";
}

function closeOverlayOnClick(element)
{
    element.addEventListener("click", function(e) {
        // close overlay when clicked into background
        if (e.target !== this) return;
        setOverlayVisible(false);
    });
}

function clearTable(table)
{
    var new_tbody = document.createElement('tbody');
    var old_tbody = table.tBodies[0];
    table.replaceChild(new_tbody, old_tbody);
}

function insertRow(table, cells)
{
    var row = table.insertRow(0);
    for(var i = 0; i < cells.length; i++)
    {
        var cellText = cells[i];
        var cell = row.insertCell(i);
        cell.innerHTML = cellText;
    }
}

function toClipboard(text)
{
  navigator.clipboard.writeText(text);
}