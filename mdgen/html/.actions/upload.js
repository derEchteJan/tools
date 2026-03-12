// upload.js

function onLoad()
{
    var initialPath = getFilePathFromUlr();
    var textarea = document.getElementById("ta_file_path");
    textarea.value = initialPath;
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