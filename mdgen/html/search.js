// search.js
import * as Utils from '/utils.js';

const s_searchBar = document.getElementById("searchbar");
const s_resultSection = document.getElementById("searchresults");
const s_resultList = document.getElementById("resultlist");
const s_overviewSection = document.getElementById("overview");
const s_resultCount = document.getElementById("resultcount");

var s_indexLoaded = false;
var s_indexContent = null;
var s_resultSectionShown = false;

OnLoad();

function OnLoad()
{
   console.log("search onLoad");
   s_searchBar.value = "";
   s_searchBar.addEventListener('input', function(event) {
      console.log("query changed");
      var query = s_searchBar.value;
      SearchFor(query);
   });
   LoadIndex();
}

function LoadIndex()
{
   Utils.Fetch("/search.index", { onReceived: OnIndexReceived });
}

/**
 * @param {boolean} success 
 * @param {string} content 
 */
function OnIndexReceived(success, content)
{
   if(success == true)
   {
      s_indexContent = content;
      s_indexLoaded = true;
      console.log("search index loaded");
   }
}

/** Starts search for given input text
 * @param {string} query - search text
 */
function SearchFor(query)
{
   if(query.length === 0)
   {
      showResultsSection(false);
   }
   else
   {
      showResultsSection(true);
      ClearResults();

      if(s_indexContent === null) return;

      query = query.toLowerCase().trim(); // TODO: split by spaces

      let lines = s_indexContent.split('\n');
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
               AddResult(displayName, url);
               matches += 1;
               break;
            }
         }
      })

      s_resultCount.innerText = "" + matches + " Results";
   }
}

/**
 * @param {string} displayName 
 * @param {string} url 
 */
function AddResult(displayName, url)
{
   // create a new div element
   const item = document.createElement("li");

   const link = document.createElement("a");
   link.href = url;
   link.innerText = displayName;
  
   item.appendChild(link);

   // add the newly created element and its content into the DOM
   //const currentDiv = document.getElementById("searchresults");
   s_resultList.appendChild(item);
}

function ClearResults()
{
   s_resultList.innerHTML = '';
}

/**
 * @param {boolean} show 
 */
function showResultsSection(show)
{
   if(show !== s_resultSectionShown)
   {
      s_resultSectionShown = show;
      s_resultSection.style.display = show ? "inline" : "none";
      s_overviewSection.style.display = !show ? "inline" : "none";
   }
}