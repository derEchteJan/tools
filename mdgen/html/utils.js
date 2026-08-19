// utils.js

/* Logging / Modal / Show Alerts */

var s_toastTimeoutId = null;
var s_toastTimeoutDur = 3000; /* in ms */
var s_overlay = document.getElementById("overlay");
var s_siteConfig = null;

export const Level = {
   INFO: 0,
   OK:   1,
   WARN: 2,
   ERR:  3,
};

export function OnLoad()
{
   // registers click listeners to close overlay
   var overlayDiv = document.getElementById("overlay");
   if(overlayDiv) CloseOverlayOnClick(overlayDiv);
   var overlayMainDiv = document.getElementById("overlay-main");
   if(overlayMainDiv) CloseOverlayOnClick(overlayMainDiv);

   LoadSiteConfig();
}

export function GetSiteConfig()
{
   return s_siteConfig;
}

function LoadSiteConfig()
{
   Fetch('/site.conf', { onReceived: OnSiteConfigReceived, params: { 'test': 'me', 'very': 'well' } })
}

function OnSiteConfigReceived(success, content)
{
   if(success == true)
   {
      var lines = content.split('\n');
      for(var line of lines)
      {
         line = line.trim();
         if(line.at(0) == '#') continue;
         var splitIndex = line.indexOf("=");
         if(splitIndex)
         {
            if(!s_siteConfig) s_siteConfig = new Map();
            var key = line.substring(0, splitIndex);
            var value = line.substring(splitIndex + 1, line.length);
            var firstc = value.at(0);
            var lastc = value.at(value.length - 1);
            if((firstc == '"' || firstc == '\'') && firstc == lastc)
            {
               value = value.substring(1, value.length - 1);
            }
            //console.log("site.conf value: " + key + " = " + value);
            s_siteConfig[key] = value;
         }
      }
   }
   console.log("site.conf loaded");
   console.log(JSON.stringify(s_siteConfig, null, 2));
}

/**
 * @param {Level} level appearance / level
 * @param {string} titleText title text
 * @param {string|null} bodyText optional body text
 * @param {string|null} logFile optional path to log file (will add link to body)
 */
export function ShowToast(level, titleText, bodyText, logFile) {
   console.log(titleText);
   if (bodyText) console.log(bodyText);

   ClearToast();

   var modalContent = document.getElementById("modal-content");
   var toast = document.createElement("div");
   toast.classList.add("toast-div");
   var levelClassId = "info";
   switch(level)
   {
      case Level.OK: levelClassId = "success"; break;
      case Level.WARN: levelClassId = "warning"; break;
      case Level.ERR: levelClassId = "error"; break;
   }
   toast.classList.add(levelClassId);
   var title = document.createElement("h3");
   title.appendChild(document.createTextNode(titleText));
   toast.appendChild(title);
   if (bodyText) {
      var body = document.createElement("p");
      body.appendChild(document.createTextNode(bodyText));
      if(logFile)
      {
         var logLink = document.createElement("a");
         logLink.href = logFile;
         logLink.text = logFile;
         body.appendChild(logLink);
      }
      toast.appendChild(body);
   }
   modalContent.appendChild(toast);

   window.setTimeout(ClearToast, s_toastTimeoutDur);
}

export function Toast(titleText)
{
   ShowToast(Level.INFO, titleText);
}

export function ClearToast() {
   if(s_toastTimeoutId)
   {
      window.clearTimeout(s_toastTimeoutId);
      s_toastTimeoutId = null;
   }
   var modalContent = document.getElementById("modal-content");
   modalContent.innerHTML = "";
}

function CloseOverlayOnClick(element) {
   element.addEventListener("click", function (e) {
      // close overlay when clicked into background
      if (e.target !== this) return;
      SetOverlayVisible(false);
   });
}

export function ShowOverlay() { SetOverlayVisible(true); }

export function HideOverlay() { SetOverlayVisible(false); }

export function SetOverlayVisible(visible) {
   s_overlay.style.display = visible === true ? "inline" : "none";
   if (visible === false)
      ClearToast();
}

/**
 * Sends XMLHttpRequest with given path and callback handler + optional url parameters
 * @param {string} path 
 * @param {{ onReceived: function(bool, string), params: object | null }} args - response handler and optional parameters
 */
export function Fetch(path, args)
{
   var request = new XMLHttpRequest();
   request.onreadystatechange = function()
   { 
      if (request.readyState == 4)
      {
         args.onReceived(request.status == 200, request.responseText);
      }
   }
   if(args.params)
   {
      var first = true;
      for(const [key, value] of Object.entries(args.params))
      {
         path += first == true ? "?" : "&";
         path += key + "=" + encodeURIComponent(value);
         first = false;
      }
   }
   request.open(/*method:*/ "GET", /*url:*/ path, /*async:*/ true);
   request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0'); // preven caching
   request.send(null);
}