<?php
// call for on-page javascript to determine wether php is avaliable / aka 'in edit mode'
header("Cache-Control: no-cache, must-revalidate"); // prevent response caching
http_response_code(200);
echo '<pre>php is enabled</pre>';
?>