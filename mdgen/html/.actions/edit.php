<?php
    $status = 409;
    $response = "";

    $mdgenBin = '/usr/bin/generator';
    $docRoot = '/usr/local/apache2/htdocs';
    $fileSuffix = '.md';

    $requestBody = file_get_contents('php://input');
    $fileParam = $_GET['file'];

    function isValid($str)
    {
        return $str && strlen($str) !== 0 && !preg_match('/[^A-Za-z0-9\.\/_-]/', $str);
    }

    if(is_null($fileParam))
    {
        $status = 409;
        $response = "url parameter 'file' missing\n";
    }
    else if(!isValid($fileParam))
    {
        $response = "invalid file name: '$fileParam'\n";
        $status = 403;
    }
    else
    {
        if(substr($fileParam, 0, 1) === "/")
        {
            $fileParam = substr($fileParam, 1);
        }
        $outputFilePath = $docRoot . '/' . $fileParam;
        $outputFilePath = str_replace('//', '/', $outputFilePath);
        $outputFilePath = str_replace('/..', '/', $outputFilePath);
        $outputFilePath = str_replace('/.', '/', $outputFilePath);

        $response .= "upload file path: $outputFilePath\n";

        // upload body into markdown file
        $result = file_put_contents($outputFilePath, $requestBody);

        if($result !== false)
        {
            $response .= "ok\n\nuploaded file: $outputFilePath\nhttp://localhost:8080/$fileParam\n";

            $output = null;
            $retval = null;
            $command = "document_root=$docRoot $mdgenBin 'file=$outputFilePath'";
            $response .= "running generator command:\n$command\n";
            exec($command, $output, $retval);

            $logfile = fopen("$docRoot/log.txt", "w");
            foreach ($output as $line)
            {
                fwrite($logfile, $line);
                fwrite($logfile, "\n");
            }

            $status = $retval == 0 ? 200 : 503;
            $response .= "generator exited with $retval\n";
        }
        else
        {
            $status = 409;
            $response .= "file_put_contents error\n";
        }
    }

    http_response_code($status);
    echo("$response");
?>