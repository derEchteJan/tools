<?php
$cmd = "sudo systemctl restart bedrock";
echo "<pre>\n";
echo "restarting bedrock server..\n";
echo "running $cmd\n";
$output = shell_exec("($cmd) 2>&1; echo exited with $?");
echo "$output";
echo "</pre>";
?>
