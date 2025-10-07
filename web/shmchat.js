
function sendPressed()
{
    console.log("sending");

    var input = document.getElementById("textIn");
    shmc_post(input.value);
}

function getPressed()
{
    console.log("getting");
    shmc_get();
}

function get_url()
{
    var debug_checkbox = document.getElementById("debug_checkbox");
    return (debug_checkbox.checked == true) ? "/shmchat.php?debug=true" : "/shmchat.php";
}

function shmc_get()
{
    var request = new XMLHttpRequest();
    request.open( "GET", get_url(), false ); // false for synchronous request
    request.send( null );
    console.log("response: \n" + request.responseText);

    var output = document.getElementById("textOut");
    output.value = request.responseText;
}

function shmc_post(text)
{
    var request = new XMLHttpRequest();
    request.open( "POST", get_url(), false );
    request.send( text );
    console.log("response: \n" + request.responseText);

    var output = document.getElementById("textOut");
    output.value = request.responseText;
}