// edit.js

function onLoad()
{
    var initialPath = getFilePathFromUlr();
    var textarea = document.getElementById("ta_file_path");
    textarea.value = initialPath;
    
    onFetchPressed();
};

function getFilePath()
{
    var filePathInput = document.getElementById("ta_file_path");
    var filePath = filePathInput.value;
    if(filePath.length === 0) 
        return '/';
    if(filePath.at(0) !== '/')
        filePath = '/' + filePath;
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
    var content = document.getElementById("ta_text_input").value;
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
    var content = document.getElementById("ta_text_input").value;
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
    document.getElementById("ta_text_input").placeholder = "loading file..."

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
    var textarea = document.getElementById("ta_text_input");
    textarea.value = content;
    textarea.placeholder = "";
}

/**
 * @param {string} content 
 */
function onDocumentFetchFailed(content)
{
    var textarea = document.getElementById("ta_text_input");
    textarea.value = "";
    textarea.placeholder = "empty file";
}

/**
 * @param {boolean} redirect 
 */
function onDocumentUploaded(redirect)
{
    var filepath = document.getElementById("ta_file_path").value;

    if(filepath.length > 0 && filepath.at(0) !== '/')
        filepath = "/" + filepath;

    var resultLink = document.getElementById("link_result");
    var originalLink = document.getElementById("link_original");
    resultLink.href = filepath;
    resultLink.textContent = filepath;

    if(redirect === true)
    {
        window.location.href = filepath.replace(".md", ".html");
    }
}