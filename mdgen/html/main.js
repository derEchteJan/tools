// main.js
import * as Utils from '/utils.js';

var s_hasPhp = false;

const s_writeModeButton = document.getElementById("btn-writemode");
const s_editButton = document.getElementById("btn-edit");
const s_createButton = document.getElementById("btn-create");
const s_spanOn = document.getElementById("edit-on");
const s_spanOff = document.getElementById("edit-off");

Utils.OnLoad();
OnLoad();

function OnLoad()
{
   if(s_editButton)
      s_editButton.onclick = OnEditPressed;
   if(s_writeModeButton)
      s_writeModeButton.onclick = OnWriteModePressed;
   if(s_createButton)
      s_createButton.onclick = OnCreatePressed;

   Utils.Fetch('/.actions/has.php?', { onReceived: OnHasPhpReceived })

   Utils.Toast("main.js initialized");
}

function OnEditPressed()
{
   if(s_hasPhp)
   {
      var path = window.location.pathname;
      window.location.href = "/.actions/edit.html?file=" + path.replace(".html", ".md");
   }
   else
   {
      Utils.ShowToast(Utils.Level.ERR, "Not in edit mode");
   }
}

function OnCreatePressed()
{
   if(s_hasPhp)
   {
      window.location.href = "/.actions/edit.html?file=" + encodeURIComponent("Neue Datei");
   }
   else
   {
      alert("You are not in edit mode");
   }
}

function OnWriteModePressed()
{
   var config = Utils.GetSiteConfig();
   if(config)
   {
      if(window.location.host.includes('localhost:'))
      {
         const roPort = config['RO_PORT'];
         const rwPort = config['RW_PORT'];
         if(roPort && rwPort)
         {
            window.location = (window.location + "").replace(roPort, rwPort);
         }
      }
      else
      {
         const roHost = config['RO_HOST'];
         const rwHost = config['RW_HOST'];
         if(roHost && rwHost)
         {
            window.location = (window.location + "").replace(roHost, rwHost);
         }
      }
   }

   var locationValue = (window.location + "");
   if(locationValue.includes("localhost"))
   {
      var config = Utils.GetSiteConfig();
      if(!config) return;
      var roPort = config['RO_PORT'];
      var rwPort = config['RW_PORT'];
      var newLocation = locationValue.replace(roPort, rwPort);
      window.location = newLocation;
   }
   else
   {
      window.location = (window.location + "").replace("rezepte.fvsion.de", "rezepte-edit.fvsion.de"); // TODO: url/port values from config file
   }
}

/** Handler for 'has php' result
 * @param {boolean} enabled - php enabled
 */
function OnHasPhpReceived(enabled)
{
   s_hasPhp = enabled;

   if(s_writeModeButton)
     s_writeModeButton.disabled = s_hasPhp;
   if(s_editButton)
     s_editButton.disabled = !s_hasPhp;
   if(s_createButton)
     s_createButton.disabled = !s_hasPhp;
   
   s_spanOn.style.display = s_hasPhp ? "inline" : "none";
   s_spanOff.style.display = !s_hasPhp ? "inline" : "none";
}