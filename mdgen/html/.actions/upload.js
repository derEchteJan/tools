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

function submitForm()
{
    sendPostRequest("testfile.txt")
}

function sendPostRequest(path)
{
    var request = new XMLHttpRequest();
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4)
        {
            console.log("status: " + request.status + "\ncontent: \n" + request.responseText);
            if(request.status == 200)
            {
                //onDocumentUploaded(redirect);
                console.log("doc uploaded");
                //console.log(request.responseText);
            }
            else
            {
                console.log("upload error"); // todo handling
            }
        }
    }
    //console.log("POST /.actions/upload.php?file=" + encodeURIComponent(path));
    request.open(/*method:*/ "POST", /*url:*/ "/.actions/upload.php?file=" + encodeURIComponent(path), /*async:*/ true);

    //request.setRequestHeader("Content-type","application/x-www-form-urlencoded");

    var formData = new FormData( document.getElementById("upload-form") );
    request.send(formData);

    //request.send(content);
}