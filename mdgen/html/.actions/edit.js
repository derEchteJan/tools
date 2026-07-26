// edit.js

var s_pathInput = document.getElementById("ta_file_path");
var s_textArea = document.getElementById("ta_text_input")

function onLoad()
{
    var initialPath = getFilePathFromUlr();
    s_pathInput.value = initialPath;
    onFetchPressed();
};

function getFilePath()
{
    var filePath = '';
    var parts = s_pathInput.value.trim().split('/');
    if(parts.length == 0)
    {
        filePath = '/';
    }
    else
    {
        for(var i in parts)
        {   
            var part = parts[i];
            if(!part) continue;
            part = part.trim();
            part = part.replaceAll(" ", "_");
            filePath += "/" + part;
        }
    }
    console.log("pre filter: " + filePath);
    var regex = /[A-Za-z0-9\.\/_-]+/g;
    filePath = (filePath.match(regex) || []).join('');
    console.log("post filter: " + filePath);
    if(!filePath.endsWith('.md'))
    {
        filePath += '.md';
    }
    return filePath;
}

function getFilePathFromUlr()
{
    const params = new URLSearchParams(window.location.search);
    var result = "";
    for (const [key, value] of params)
    {
        if(key == "file") result = value;
    }
    return result;
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
    //if(filepath.startsWith('/'))
    //    filepath = filepath.substring(1);

    var resultLink = document.getElementById("link_result");
    resultLink.href = filepath;
    resultLink.textContent = filepath;

    if(redirect === true)
    {
        window.location = filepath.replace(".md", ".html");
    }
}