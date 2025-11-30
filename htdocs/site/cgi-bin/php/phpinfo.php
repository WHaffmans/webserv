#!/usr/bin/env php-cgi
<?php
// phpinfo() outputs full HTML including its own <head> and <body>
// We want JUST the inside content.

ob_start();
phpinfo();
$html = ob_get_clean();

// Strip everything before <body> and after </body>
if (preg_match('/<body[^>]*>(.*)<\/body>/is', $html, $match)) {
    $body = $match[1];
} else {
    $body = $html;
}

header("Content-Type: text/html; charset=UTF-8");

// IMPORTANT: Clean up phpinfo() table styling to match your theme
$body = str_replace("table", "table class=\"min-w-full text-xs border border-border mt-4\"", $body);
$body = str_replace("<tr>", "<tr class=\"border-b border-border\">", $body);
$body = str_replace("<th", "<th class=\"bg-secondary text-left px-2 py-1 font-medium border border-border\"", $body);
$body = str_replace("<td", "<td class=\"px-2 py-1 border border-border text-foreground\"", $body);

// Remove ugly background colors from phpinfo output
$body = preg_replace('/bgcolor="[^"]*"/i', '', $body);

echo $body;
