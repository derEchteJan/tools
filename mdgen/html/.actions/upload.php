<?php
  // uploads file to .attachments supdirectory

  $response = "";
  $status = 409;

  /// functionality requires file_uploads = On in active php.ini

  $docRoot = "/usr/local/apache2/htdocs";
  $attachmentSubdir = ".attachments";

  if(ini_get('file_uploads') != 1)
  {
    $status = 503;
    $response .= "file uploads are disabled, check php.ini";
    goto end;
  }

  $dirParam = $_GET["dir"];
  $targetDir = $docRoot;
  $targetName = basename($_FILES["fileToUpload"]["name"]);
  if($dirParam) $targetDir .= $dirParam;
  $targetDir .= "/" . $attachmentSubdir;
  $targetFile = "$targetDir" . "/" . "$targetName";

  function isValid($str)
  {
      return $str && strlen($str) !== 0 && !preg_match('/[^A-Za-z0-9\.\/_-]/', $str);
  }

  //echo "targetDir: '$targetDir'\n";
  //echo "targetFile: '$targetFile'\n";

  $response .= "targetFile: '$targetFile'\n";

  if(!isValid($targetFile))
  {
    $status = 403;
    $response .= "invalid file name";
    goto end;
  }

  if(!is_dir($targetDir))
  {
    $created = mkdir(
      $targetDir,
      0777,
      true
    );
    if($created)
    {
      $response .= "created directory '" . $targetDir . "'\n";
    }
    else
    {
      $status = 403;
      $response .= "unable to create directory '" . $targetDir . "'";
      goto end;
    }
  }

  // move uploaded file to tartget dir
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $targetFile))
  {
    $status = 200;
    $response .= "uploaded file '" . $targetFile . "'\n";
    $url = substr($targetFile, 0, strlen($docRoot));
    $response .= "url: <a href=\"" . $url .  "\">" . $url . "</a>\n";
    goto end;
  }
  else
  {
    echo "Sorry, there was an error uploading your file.";
    $status = 403;
    $response .= "error moving file";
    goto end;
  }

end:
  http_response_code($status);
  header("Cache-Control: no-cache, must-revalidate"); // prevent response caching
  echo("<pre>");
  echo("$response");
  echo("</pre>");
?>
