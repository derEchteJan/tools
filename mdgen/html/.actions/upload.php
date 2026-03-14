<?php

/// functionality requires file_uploads = On in active php.ini

if(ini_get('file_uploads') != 1)
{
  echo 'FILE UPLOADS ARE DISABLED!';
  return;
}

$ok = 1;

$target_dir = "/usr/local/apache2/htdocs/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);

echo "targetFile: $target_file\n";

// Check if image file is a actual image or fake image
$imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
if(isset($_POST["submit"]))
{
  $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
  if($check !== false)
  {
    // File is an image
    echo "File is an image - " . $check["mime"] . ".";
  }
  else
  {
    // Regular file
    echo "File is not an image.";
  }
}

if ($ok == 1)
{
  // move uploaded file to tartget dir
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file))
  {
    echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.";
    echo "Full name: " . $_FILES["fileToUpload"]["name"];
    $ok = 1;
  }
  else
  {
    echo "Sorry, there was an error uploading your file.";
    $ok = 0;
  }
}

if($ok == 1)
{
    http_response_code(200);
}
else
{
    http_response_code(403);
}

?>
