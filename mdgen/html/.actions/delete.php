<?php
  // uploads file to .attachments supdirectory

   $response = "";
   $status = 409;

   $docRoot = "/usr/local/apache2/htdocs";
   $trashSubdir = ".trash";
   $trashRoot = $docRoot . "/" . $trashSubdir;
   $pagesDir = 'pages';
   $pageSuffix = '.md';
   $auxTypes = ['.html', '.index'];

   function validFileName($str)
   {
      return $str && strlen($str) !== 0 && !preg_match('/[^a-z0-9\.\/_-]|\.\.|\/\//i', $str);
   }

   function validDirName($str)
   {
      return $str && strlen($str) !== 0 && !preg_match('/[^a-z0-9\/_-]|\.\.|\/\//i', $str);
   }

   $fileParam = $_GET["file"];
   $sourceFile = $docRoot . '/' . $pagesDir . '/' . $fileParam;
   $targetFile = $trashRoot . '/' . $fileParam;
   $targetDir = null;
   $auxFiles = [];
   if(str_ends_with($sourceFile, $pageSuffix))
   {
      $baseName = substr($sourceFile, 0, strlen($sourceFile) - strlen($pageSuffix));
      foreach($auxTypes as $auxType)
      {
         $auxFiles[] = $baseName . $auxType;
      }
   }

   $response .= "deleting: '$sourceFile'\n";

   if(!validFileName($sourceFile))
   {
      $status = 403;
      $response .= "invalid file name\n";
      goto end;
   }

   $lastPos = strrpos($targetFile, "/", strlen($docRoot) + 1);
   if($lastPos != false)
   {
      $dirLen = $lastPos - $baseLen + 1;
      $targetDir = substr($targetFile, 0, $lastPos);
   }

   if(!file_exists($sourceFile) || is_dir($sourceFile))
   {
      $status = 403;
      $response .= "source file doesnt exist\n";
      goto end;
   }

   if($targetDir && !is_dir($targetDir))
   {
      $created = mkdir(
         $targetDir,
         0777,
         true
      );
      if($created)
      {
         $response .= "created directory '$targetDir'\n";
      }
      else
      {
         $status = 403;
         $response .= "unable to create directory '$targetDir'\n";
         goto end;
      }
   }

   if(!copy($sourceFile, $targetFile))
   {
      $status = 500;
      $response .= "unable to copy file from '$sourceFile' to '$targetFile'\n";
      goto end;
   }

   if(!unlink($sourceFile))
   {
      $status = 500;
      $response .= "unable to delete file '$sourceFile'";
      goto end;
   }

   foreach($auxFiles as $auxFile)
   {
      if(file_exists($auxFile))
      {
         if(unlink($auxFile))
         {
            $response .= "deleted aux file '$auxFile'\n";
         }
         else
         {
            $response .= "unable to delete aux file '$auxFile'\n";
         }
      }
   }

   $status = 200;
   $response .= "deleted file\n";

end:
  http_response_code($status);
  header("Cache-Control: no-cache, must-revalidate"); // prevent response caching
  echo("<pre>");
  echo("$response");
  echo("</pre>");
?>
