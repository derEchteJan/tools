<?php
   // runs generator on all pages

   // TODO: needed by edit.php as well, find out how to include
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

   $mdgenBin = '/usr/bin/generator';
   $docRoot = '/usr/local/apache2/htdocs';
   $configFile = "$docRoot/site.conf";
   $logFile = "$docRoot/log.txt";
   $pageDir = 'pages';

   // Load site.conf values
   $conf = GetSiteConfig($configFile);

   // Run generator command
   $command = "";
   $output = null;
   $retval = null;

   $command = "document_root='$docRoot' ";
   if($conf["SITE_NAME"]) $command .= "site_name='".$conf["SITE_NAME"]."' ";
   if($conf["THEME"])     $command .= "theme='".    $conf["THEME"]."' ";
   if($conf["VERBOSE"])   $command .= "verbose=".   $conf["VERBOSE"]." ";
   $command .= "$mdgenBin 'dir=$pageDir'";

   $response .= "running generator command:\n$command\n";
   exec($command, $output, $retval);

   $status = $retval == 0 ? 200 : 503;
   $response .= "generator exited with $retval\n";

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

end:
   http_response_code($status);
   echo("<pre>\n");
   echo("$response");
   echo('output logfile: <a href="/log.txt">log.txt</a>');
   echo("\n</pre>");
?>