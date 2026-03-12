<?php

/// requires file_uploads = On in php.ini

$inipath = php_ini_loaded_file();
if ($inipath) {
    echo 'Loaded php.ini: ' . $inipath;
} else {
    echo 'A php.ini file is not loaded';
}

echo 'file_uploads:';
echo ini_get('file_uploads');

echo '$_FILES:';
var_dump($_FILES);


$target_dir = "";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
# mark jan: test
$target_file = $target_file . "jeff.svg";
echo "targetFile: $target_file\n";
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));
// Check if image file is a actual image or fake image
if(isset($_POST["submit"])) {
  $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
  if($check !== false) {
    echo "File is an image - " . $check["mime"] . ".";
    $uploadOk = 1;
  } else {
    echo "File is not an image.";
    //$uploadOk = 0;
  }
}

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  echo "Sorry, your file was not uploaded.";
// if everything is ok, try to upload file
} else {
    echo("files: \n");
    echo($_FILES["fileToUpload"]);
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
    echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.";
    $uploadOk = 1;
  } else {
    echo "Sorry, there was an error uploading your file.";
    $uploadOk = 0;
  }
}

if($uploadOk == 1)
{
    http_response_code(200);
}
else
{
    http_response_code(403);
}

?>
