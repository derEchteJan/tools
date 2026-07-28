<?php
    // lists file in given dir (GET param 'dir')

    $status = 409;
    $response = "";

    // TODO: move constants to a common file
    $docRoot = '/usr/local/apache2/htdocs';

    $dir = $_GET['dir'];

    function isValid($str)
    {
        return $str && strlen($str) !== 0 && !preg_match('/[^A-Za-z0-9\.\/_-]/', $str);
    }

    if(is_null($dir))
    {
        $status = 409;
        $response = 'dir param missing';
    }
    else if(!isValid($dir))
    {
        $status = 409;
        $response = "dir param missing";
    }
    else
    {
        $path = $docRoot . $dir;
        if(is_dir($path))
        {
            $files = scandir($path);
            if(!$files)
            {
                $status = 503;
                $response = "unable to list files in '" . $path . "'";
            }
            else
            {
                $status = 200;
                $response = "";
                foreach($files as $file)
                {
                    if(is_dir($path . '/' . $file)) continue;
                    $response .= "$file" . ";";
                }
            }
        }
        else
        {
            $status = 409;
            $response = "dir param is not a directory";
        }
    }

    http_response_code($status);
    //echo("<pre>");
    echo($response);
    //echo("</pre>");
?> 