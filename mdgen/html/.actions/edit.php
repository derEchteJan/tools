<?php
    $status = 409;
    $response = "";
    $result = "";

    $mdgenBin = '/usr/bin/generator';
    $docRoot = '/usr/local/apache2/htdocs';
    $fileSuffix = '.md';

    $requestBody = file_get_contents('php://input');
    $fileParam = $_GET['file'];

    if($fileParam && strlen($fileParam) > 0)
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

        // TODO: input cleanup

        // upload body into markdown file
        $result = file_put_contents(
            $outputFilePath,
            $requestBody
        );

        if($result !== false)
        {
            $status = 200;
            $response .= "ok\n\nuploaded file: $outputFilePath\nhttp://localhost:8080/$fileParam\n";

            $output = null;
            $retval = null;
            $command = "document_root=$docRoot $mdgenBin file=$outputFilePath";
            $response .= "running generator command:\n$command\n";
            exec($command, $output, $retval);

            $logfile = fopen("$docRoot/log.txt", "w");
            foreach ($output as $line)
            {
                fwrite($logfile, $line);
                fwrite($logfile, "\n");
            }

            if($retval == 0)
            {
                $status = 200;
            }
            else
            {
                $status = 503; // generator exception
            }
            $response .= "generator exited with $retval\n";
        }
        else
        {
            $status = 409;
            $response .= "file_put_contents error\n";
        }
    }
    else
    {
        $status = 409;
        $response .= "url parameter 'file' missing\n";
    }

    http_response_code($status);
    echo("$response");
?>