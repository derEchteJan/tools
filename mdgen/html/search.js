

let searchBar = document.getElementById("searchbar");
let resultSection = document.getElementById("searchresults");
let resultList = document.getElementById("resultlist");
let overviewSection = document.getElementById("overview");
let resultCount = document.getElementById("resultcount");

var indexLoaded = false;
var indexContent = null;
var resultSectionShown = false;

if(window.addEventListener)
    window.addEventListener('load', search_onLoad, false); //W3C
else
    window.attachEvent('onload', search_onLoad); //IE

function search_onLoad()
{
    console.log("search onLoad");
    searchBar.addEventListener('input', function(event) {
        console.log("query changed");
        var query = searchBar.value;
        searchFor(query);
    });
    loadIndex();
}

function loadIndex()
{
    fetchDocument("/search.index");
}

function fetchDocument(path)
{
    //document.getElementById("ta_text_input").placeholder = "loading file..."

    var request = new XMLHttpRequest();
    request.onreadystatechange = function()
    { 
        if (request.readyState == 4 )
        {
            if(request.status == 200)
            {
                onIndexReceived(true, request.responseText);
            }
            else
            {
                onIndexReceived(false, null);
            }
        }
    }
    request.open(/*method:*/ "GET", /*url:*/ path, /*async:*/ true);
    request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
    request.send(null);
}

/**
 * @param {boolean} success 
 * @param {string} content 
 */
function onIndexReceived(success, content)
{
    console.log("received index");
    console.log("success: " + success);
    if(success === true)
    {
        console.log("content:");
        console.log(content);
        indexContent = content;
        indexLoaded = true;
    }
}

/**
 * @param {string} query 
 */
function searchFor(query)
{
    if(query.length === 0)
    {
        showResultsSection(false);
    }
    else
    {
        showResultsSection(true);
        clearResults();

        if(indexContent === null) return;

        query = query.toLowerCase().trim(); // TODO: split by spaces

        let lines = indexContent.split('\n');
        var matches = 0;

        lines.forEach((line) => { 
            let items = line.split(',');

            if(items.length < 2) return;

            let url = items[0];
            let displayName = items[1];

            for(var i = 0; i < items.length; i++)
            {
                let item = items[i].toLowerCase().trim();
                if(item.includes(query))
                {
                    addResult(displayName, url);
                    matches += 1;
                    break;
                }
            }
        })

        resultCount.innerText = "" + matches + " Results";
    }
}

/**
 * @param {string} displayName 
 * @param {string} url 
 */
function addResult(displayName, url)
{
  // create a new div element
  const item = document.createElement("li");

  const link = document.createElement("a");
  link.href = url;
  link.innerText = displayName;
  
  item.appendChild(link);

  // add the newly created element and its content into the DOM
  //const currentDiv = document.getElementById("searchresults");
  resultList.appendChild(item);
}

function clearResults()
{
    resultList.innerHTML = '';
}

/**
 * @param {boolean} show 
 */
function showResultsSection(show)
{
    if(show !== resultSectionShown)
    {
        resultSectionShown = show;
        resultSection.style.display = show ? "inline" : "none";
        overviewSection.style.display = !show ? "inline" : "none";
    }
}