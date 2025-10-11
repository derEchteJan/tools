<?php

$DEBUG = false;
$method = $_SERVER['REQUEST_METHOD'];
$cmd = '';
$args = '';

if($_GET['debug'] == "true") $DEBUG = true;

if($method == "GET")
{
    $cmd = "shmchat get bedrock_server1";
}
elseif($method == "POST")
{
    $cmd = "shmchat send bedrock_server0";
    $body = file_get_contents('php://input');
    $args = $body;
}
else
{
    header("HTTP/1.1 405 METHOD_NOT_ALLOWED");
    echo "method not allowed";
    return;
}

$cmd = "$cmd \"$args\""; // TODO: proper input cleanup or hack

if($DEBUG)
{
    echo "<pre>\n";
    echo "running '$cmd'\n";
    $output = shell_exec("($cmd) 2>&1; echo exited with $?");
    echo "$output";
    echo "</pre>";
}
else
{
    $output = shell_exec("($cmd)");
    echo "$output";
}
?>
