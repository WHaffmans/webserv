#!/usr/bin/env php-cgi
<?php
// Runs posted PHP code in an isolated output buffer.
// NOTE: This is for demo only (do not expose on public internet).
header("Content-Type: text/plain");

// read POST body (application/x-www-form-urlencoded)
$code = $_POST['code'] ?? '';
if (trim($code) === '') {
    echo "ERROR: No code provided.\n";
    exit;
}

// Run in output buffer
ob_start();
try {
    // Remove opening/closing tags if the user included them
    $code = preg_replace('/^\s*<\?php/i', '', $code);
    $code = preg_replace('/\?>\s*$/', '', $code);
    eval($code);
} catch (Throwable $e) {
    echo "ERROR:\n" . $e . "\n";
}
$out = ob_get_clean();
echo $out;
