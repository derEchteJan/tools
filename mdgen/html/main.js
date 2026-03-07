// main.js

function onLoad()
{
}

function onEditPressed()
{
    var path = window.location.pathname;
    window.location.href = "/.actions/edit.html?file=" + path.replace(".html", ".md");
}