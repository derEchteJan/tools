<?php
    // runs generator on all pages

    $mdgenBin = '/usr/bin/generator';
    $docRoot = '/usr/local/apache2/htdocs';
    $pageDir = 'pages';

    $output = null;
    $retval = null;
    $command = "document_root=$docRoot $mdgenBin 'dir=$pageDir'";

    $verbose = $_GET['v']; // TODO: input cleanup or heck
    if($verbose)
        $command .= " 'verbose=$verbose'";

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

    http_response_code($status);
    echo("<pre>\n");
    echo("$response");
    echo('output logfile: <a href="/log.txt">log.txt</a>');
    echo("\n</pre>");
?>