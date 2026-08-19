// edit.js
import * as Utils from '../utils.js';

var s_pathInput = document.getElementById("ta_file_path");
var s_textArea = document.getElementById("ta_text_input");

const s_imageFormats = ["png", "jpeg", "jpg", "webp", "gif", "bmp"];
const s_overviewPage = "/index.html";
const s_generatorLogFile = "/log.txt";
const s_attachmentsDir = ".attachments";
const s_pagesDir = "pages";

Utils.OnLoad();
OnLoad();

function OnLoad()
{
   document.getElementById("btn-save").onclick = savePressed;
   document.getElementById("btn-cancel").onclick = cancelPressed;
   document.getElementById("btn-delete").onclick = askToDelete;
   document.getElementById("btn-reload").onclick = reloadPressed;
   document.getElementById("btn-overlay").onclick = overlayPressed;
   document.getElementById("btn-submit-att").onclick = SubmitAttachment;

   SetFilePathFromUlr();

   onFetchPressed();

   // debug
   Utils.Toast("edit.js initialized");
};

/**
 * Escapes disallowed chars in file path components
 * @param {string} str input
 * @returns {string} escaped input
 */
function EscapePathComponent(str)
{
   str = str.replaceAll(" ", "_");
   str = str.replaceAll("ß", "ss");
   return str;
}

function getFilePath(skipLastComponent) {
   var filePath = '';
   var parts = s_pathInput.value.trim().split('/');
   for (var i = 0; i < parts.length; i++) {
      var part = parts[i];
      if (!part) continue;
      if (i == parts.length - 1 && skipLastComponent == true) continue;
      part = part.trim();
      part = EscapePathComponent(part);
      if(i != 0) filePath += '/';
      filePath += part;
   }
   var regex = /[A-Za-z0-9\.\/_-]+/g;
   filePath = (filePath.match(regex) || []).join('');
   if (!filePath.endsWith('.md') && !skipLastComponent) {
      filePath += '.md';
   }
   return filePath;
}

/**
 * Sets file path from url parameter into s_pathInput value
 */
function SetFilePathFromUlr() {
   const params = new URLSearchParams(window.location.search);
   var result = "";
   for (const [key, value] of params) {
      if (key == "file") result = value;
   }
   var pref1 = '/' + s_pagesDir + '/';
   var pref2 = '/';
   if(result.startsWith(pref1)) result = result.substring(pref1.length);
   if(result.startsWith(pref2)) result = result.substring(pref2.length);
   s_pathInput.value = result;
}

// Button Actions

function overlayPressed()
{
   Utils.ShowOverlay();
   LoadAttachments();
}

function savePressed() {
   var content = s_textArea.value;
   var filePath = getFilePath();
   PostDocument(filePath, content, true);
}

function askToDelete()
{
   if(window.confirm("Delete page?")) deletePressed();
}

function deletePressed()
{
   var path = getFilePath();
   deleteDocument(path);
}

function reloadPressed() {
   onFetchPressed();
}

function cancelPressed() {
   window.history.go(-1);
}

// debug button only
//function onPostPressed() {
//   var content = s_textArea.value;
//   var filePath = getFilePath();
//   postDocument(filePath, content);
//}

function onFetchPressed() {
   var filePath = '/' + s_pagesDir + '/' + getFilePath();
   FetchDocument(filePath);
}

// API Requests

/** Uploads markdown doc contents
 * @param {string} path - markdown file path to save to
 * @param {string} content - markdown file content to save
 * @param {bool|null} redirect - redirect user back to original web page after upload success
 */
function PostDocument(path, content, redirect) {
   var request = new XMLHttpRequest();
   request.onreadystatechange = function () {
      if (request.readyState == 4) {
         if (request.status == 200)
         {
            Utils.ShowToast(Utils.Level.OK, "Document Uploaded");
            onDocumentUploaded(redirect);
         }
         else
         {
            Utils.ShowToast(Utils.Level.ERR, "Upload Error", request.responseText); // todo handling on upload error
         }
      }
   }
   request.open(/*method:*/ "POST", /*url:*/ "/.actions/edit.php?file=" + encodeURIComponent(path), /*async:*/ true);
   request.send(content);
}

/** Loads contents of markdown file and sets them into the editing textarea
 * @param {string} path - markdown file path to load from
 */
function FetchDocument(path) {
   s_textArea.placeholder = "loading file..."
   var request = new XMLHttpRequest();
   request.onreadystatechange = function () {
      if (request.readyState == 4) {
         OnDocumentReceived(request.status == 200, request.responseText);
      }
   }
   request.open(/*method:*/ "GET", /*url:*/ path, /*async:*/ true);
   request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
   request.send(null);
}

function deleteDocument(file)
{
   var request = new XMLHttpRequest();
   request.onreadystatechange = function() {
      if (request.readyState == 4)
      {
         if (request.status == 200)
         {
            generateAll();
         }
         else
         {
            Utils.ShowToast(Utils.Level.ERR, "Error deleting document", request.responseText);
         }
      }
   }
   request.open(/*method:*/ "GET", /*url:*/ "/.actions/delete.php?file=" + encodeURIComponent(file), /*async:*/ true);
   request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
   request.send(null);
}

function generateAll()
{
   var request = new XMLHttpRequest();
   request.onreadystatechange = function() {
      if (request.readyState == 4)
      {
         if (request.status == 200)
         {
            window.location = s_overviewPage;
         }
         else
         {
            Utils.ShowToast(Utils.Level.ERR, "Error generating pages", "generator exited with non-zero exit code", s_generatorLogFile);
         }
      }
   }
   request.open(/*method:*/ "GET", /*url:*/ "/.actions/generate.php?", /*async:*/ true);
   request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
   request.send(null);
}

/**
 * @param {string} content 
 */
function OnDocumentReceived(success, content) {
   if(success == true)
   {
      s_textArea.value = content;
      s_textArea.placeholder = "";
   }
   else
   {
      s_textArea.value = "";
      s_textArea.placeholder = "empty file"; // or non existent
   }

}

/**
 * @param {boolean} redirect 
 */
function onDocumentUploaded(redirect) {
   var filepath = '/' + s_pagesDir + '/' + ( getFilePath().replace(".md", ".html") );

   var resultLink = document.getElementById("link_result");
   resultLink.href = filepath;
   resultLink.textContent = filepath;

   if (redirect == true) {
      window.location = filepath.replace(".md", ".html") + "?reload=" + Math.floor(Math.random() * 10000); // random url param to force reload when navigating back to original page
   }
}

/* ----------------------------------------- */
/* Attachment Upload Overlay                 */
/* ----------------------------------------- */

function SubmitAttachment() {
   var form = document.getElementById("upload-form");
   PostFormRequest(form, OnPostAttachmentReceived);
}

function OnPostAttachmentReceived(success, responseText) {
   if (success == true)
   {
      Utils.ShowToast(Utils.Level.OK, "Document uploaded");
      LoadAttachments();
      var form = document.getElementById("upload-form");
      ClearForm(form);
   }
   else
   {
      Utils.ShowToast(Utils.Level.ERR, "Document upload error", responseText);
   }
}

function PostFormRequest(form, onReceived) {
   var formData = new FormData(form);
   var dir = getFilePath(true);
   var request = new XMLHttpRequest();
   request.onreadystatechange = function () {
      if (request.readyState == 4) {
         onReceived(request.status == 200, request.responseText);
      }
   }
   request.open("POST", "/.actions/upload.php?dir=" + encodeURIComponent(dir));
   request.send(formData);
}

function ClearForm(form) {
   var inputs = form.elements[0];
   for (var i = 0; i < inputs.length; i++) {
      var input = inputs[i];
      input.value = "";
   }
}

function Fetch2(path, onReceived) {
   s_textArea.placeholder = "loading file..."

   var request = new XMLHttpRequest();
   request.onreadystatechange = function () {
      if (request.readyState == 4) {
         if (request.status == 200) {
            onReceived(true, request.responseText);
         }
         else {
            onReceived(false, request.responseText);
         }
      }
   }
   request.open(/*method:*/ "GET", /*url:*/ path, /*async:*/ true);
   request.setRequestHeader('Cache-Control', 'no-cache, no-store, max-age=0');
   request.send(null);
}

function LoadAttachments() {
   var dirPath = '/' + s_pagesDir + '/' + getFilePath(/*skipLast:*/ true) + '/' + s_attachmentsDir;
   Fetch2("/.actions/ls.php?dir=" + encodeURIComponent(dirPath), OnLoadAttachmentsReceived);
}

function OnLoadAttachmentsReceived(success, responseText) {
   if (success == true) {
      parseAttachmentList(responseText);
   }
   else {
      Utils.ShowToast(Utils.Level.ERR, "Loading Attachments failed", "response: '" + responseText + "'");
   }
}

function parseAttachmentList(rawValue) {
   var list = rawValue.split(";");

   var placeholder = document.getElementById("no-attachments");
   var table = document.getElementById("attachment-table");
   clearTable(table);

   var basePath = getFilePath(true);

   var itemsAdded = 0;
   for (var i = 0; i < list.length; i++) {
      var item = list[i].trim();
      if (item.length > 0) {
         var sourceLink = "<a href=\"" + basePath + "/" + s_attachmentsDir + "/" + item + "\">source</a>";
         var isImage = false;
         var parts = item.split(".");
         if (parts.length > 0) {
            var suffix = parts[parts.length - 1];
            for (var type of s_imageFormats)
               if (type == suffix)
                  isImage = true;
         }

         var clipboardText = (isImage ? "![" : "[") + item + "](" + (isImage ? "" : (s_attachmentsDir + "/")) + item + ")";
         var copyButton = "<button onclick=\"toClipboard('" + clipboardText + "')\">Copy code</button>";
         insertRow(table, [item, sourceLink, copyButton]);
         itemsAdded++;
      }
   }

   // show/hide placeholder vs table depending on list empty or not
   placeholder.style.display = itemsAdded > 0 ? "none" : "inline";
   table.style.display = itemsAdded > 0 ? "inline" : "none";
}

function clearTable(table) {
   var new_tbody = document.createElement('tbody');
   var old_tbody = table.tBodies[0];
   table.replaceChild(new_tbody, old_tbody);
}

function insertRow(table, cells) {
   var row = table.insertRow(0);
   for (var i = 0; i < cells.length; i++) {
      var cellText = cells[i];
      var cell = row.insertCell(i);
      cell.innerHTML = cellText;
   }
}

function toClipboard(text) {
   navigator.clipboard.writeText(text);
}