<?php
   // called by edit.html to upload a markdown file and parse it

   $status = 409;
   $response = "";

   $mdgenBin = '/usr/bin/generator';
   $docRoot = '/usr/local/apache2/htdocs';
   $configFile = "$docRoot/site.conf";
   $logFile = "$docRoot/log.txt";
   $pagesDir = 'pages';
   $fileSuffix = '.md';

   $requestBody = file_get_contents('php://input');
   $fileParam = $_GET['file'];
   $outputFile = "$docRoot/$pagesDir/$fileParam";
   $targetDir = null;

   function GetSiteConfig($file)
   {
     $result = [];
     // Read key-value pairs from config file
     $fd = fopen($file, "r");
     if ($fd) {
       while (($line = fgets($fd)) !== false) {
         $split = strpos($line, '=');
         if($split !== false)
         {
            $key = substr($line, 0, $split);
            $value = substr($line, $split + 2, strlen($line) - 4 - $split);
            $result[$key] = $value;
         }
       }
       fclose($fd);
     }
     // Read key-value pairs from url parameters
     foreach ($_GET as $key => $value)
     {
       $result[strtoupper($key)] = $value;
     }
     return $result;
   }

   function validFileName($str)
   {
     return $str && strlen($str) !== 0 && !preg_match('/[^a-z0-9\.\/_-]|\.\.|\/\//i', $str);
   }

   function validDirName($str)
   {
     return $str && strlen($str) !== 0 && !preg_match('/[^a-z0-9\/_-]|\.\.|\/\//i', $str);
   }

   if(is_null($fileParam))
   {
     $status = 409;
     $response = "url parameter 'file' missing\n";
     goto end;
   }
   
   if(!validFileName($outputFile))
   {
     $status = 403;
     $response .= "invalid file name: '$outputFile'\n";
     goto end;
   }

   $response .= "upload file path: '$outputFile'\n";

   $lastPos = strrpos($outputFile, "/", strlen($docRoot) + 1);
   if($lastPos != false)
   {
     $targetDir = substr($outputFile, 0, $lastPos);
     echo "targetDir: '$targetDir'\n";
   }
   else
   {
     echo("no targetdir");
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
       $response .= "created directory '" . $targetDir . "'\n";
     }
     else
     {
       $status = 403;
       $response .= "unable to create directory '" . $targetDir . "'";
       goto end;
     }
   }

   // upload body into markdown file
   $result = file_put_contents($outputFile, $requestBody);

   if(!$result)
   {
      $status = 409;
      $response .= "file_put_contents error\n";
      goto end;
   }

   $response .= "ok\n\nuploaded file: $outputFile\nhttp://localhost:8080/$fileParam\n";

   // Run generator on single file
   // Load site.conf values
   $conf = GetSiteConfig($configFile);

   // Run generator command
   $command = "";
   $output = null;
   $retval = null;

   $command = "document_root='$docRoot' ";
   if($conf["SITE_NAME"]) $command .= "site_name='".$conf["SITE_NAME"]."' ";
   if($conf["THEME"])    $command .= "theme='".   $conf["THEME"]."' ";
   if($conf["VERBOSE"])   $command .= "verbose=".   $conf["VERBOSE"]." ";
   $command .= "$mdgenBin 'file=$outputFile'";
   
   $response .= "running generator command:\n$command\n";
   exec($command, $output, $retval);

   // Write stdout of command to logfile
   $fd = fopen($logFile, "w");
   if($fd)
   {
      foreach ($output as $line)
      {
         fwrite($fd, $line);
         fwrite($fd, "\n");
      }
      fclose($fd);
   }

   $status = $retval == 0 ? 200 : 503;
   $response .= "generator exited with $retval\n";

end:
   http_response_code($status);
   header("Cache-Control: no-cache, must-revalidate"); // prevent response caching
   echo("<pre>");
   echo("$response");
   echo("</pre>");
?>