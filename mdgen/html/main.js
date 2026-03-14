// main.js

function onLoad()
{
}

function onEditPressed()
{
    var path = window.location.pathname;
    window.location.href = "/.actions/edit.html?file=" + path.replace(".html", ".md");
}

function onCreatePressed()
{
    window.location.href = "/.actions/edit.html?file=Neue%20Datei";
}

function onWriteModePressed()
{
    window.location = (window.location + "").replace("rezepte.fvsion.de", "rezepte-edit.fvsion.de");
}