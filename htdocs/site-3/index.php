<?php
header("Content-Type: text/html; charset=UTF-8");
header("Cache-Control: no-cache, no-store, must-revalidate");
header("Pragma: no-cache");
header("Expires: 0");
header("X-Content-Type-Options: nosniff");
session_start();
// Initialize counter if not set
if (!isset($_SESSION['request_count'])) {
    $_SESSION['request_count'] = 0;
}

// Increment on each request
$_SESSION['request_count']++;

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Capabilities Demo</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            background: white;
            border-radius: 8px;
            padding: 20px;
            margin: 20px 0;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        h1 { color: #333; text-align: center; }
        h2 { color: #666; border-bottom: 2px solid #eee; padding-bottom: 5px; }
        table { width: 100%; border-collapse: collapse; margin: 10px 0; }
        th, td { padding: 8px; border: 1px solid #ddd; text-align: left; }
        th { background-color: #f8f9fa; font-weight: bold; }
        tr:nth-child(even) { background-color: #f8f9fa; }
        .success { color: #28a745; font-weight: bold; }
        .info { background-color: #d1ecf1; padding: 10px; border-radius: 4px; margin: 10px 0; }
        .code { background-color: #f8f9fa; padding: 10px; border-radius: 4px; font-family: monospace; }
    </style>
</head>
<body>
    <h1>🚀 CGI Capabilities Demonstration</h1>
    
    <div class="info">
        <strong>CGI Status:</strong> <span class="success">✅ PHP CGI is working!</span><br>
        <strong>Execution Time:</strong> <?php echo date('Y-m-d H:i:s'); ?><br>
        <strong>PHP Version:</strong> <?php echo phpversion(); ?><br>
        <strong>Session Request Count:</strong> <?php echo $_SESSION['request_count']; ?>
    </div>

    <div class="container">
        <h2>📊 Server Information</h2>
        <table>
            <tr><th>Property</th><th>Value</th></tr>
            <tr><td>Server Software</td><td><?php echo $_SERVER['SERVER_SOFTWARE'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Server Name</td><td><?php echo $_SERVER['SERVER_NAME'] ?? 'localhost'; ?></td></tr>
            <tr><td>Server Port</td><td><?php echo $_SERVER['SERVER_PORT'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Document Root</td><td><?php echo $_SERVER['DOCUMENT_ROOT'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Script Name</td><td><?php echo $_SERVER['SCRIPT_NAME'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Gateway Interface</td><td><?php echo $_SERVER['GATEWAY_INTERFACE'] ?? 'Unknown'; ?></td></tr>
        </table>
    </div>

    <div class="container">
        <h2>🌐 HTTP Request Information</h2>
        <table>
            <tr><th>Property</th><th>Value</th></tr>
            <tr><td>Request Method</td><td><?php echo $_SERVER['REQUEST_METHOD'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Request URI</td><td><?php echo $_SERVER['REQUEST_URI'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Query String</td><td><?php echo $_SERVER['QUERY_STRING'] ?? 'None'; ?></td></tr>
            <tr><td>Content Type</td><td><?php echo $_SERVER['CONTENT_TYPE'] ?? 'Not specified'; ?></td></tr>
            <tr><td>Content Length</td><td><?php echo $_SERVER['CONTENT_LENGTH'] ?? 'Not specified'; ?></td></tr>
            <tr><td>User Agent</td><td><?php echo $_SERVER['HTTP_USER_AGENT'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Remote Address</td><td><?php echo $_SERVER['REMOTE_ADDR'] ?? 'Unknown'; ?></td></tr>
            <tr><td>Remote Host</td><td><?php echo $_SERVER['REMOTE_HOST'] ?? 'Unknown'; ?></td></tr>
        </table>
    </div>

    <div class="container">
        <h2>🔧 CGI Environment Variables</h2>
        <table>
            <tr><th>Variable</th><th>Value</th></tr>
            <?php
            $cgi_vars = [
                'PATH_INFO', 'PATH_TRANSLATED', 'SCRIPT_NAME', 'SCRIPT_FILENAME',
                'SERVER_NAME', 'SERVER_PORT', 'SERVER_PROTOCOL', 'SERVER_SOFTWARE',
                'GATEWAY_INTERFACE', 'REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH',
                'HTTP_ACCEPT', 'HTTP_ACCEPT_ENCODING', 'HTTP_ACCEPT_LANGUAGE',
                'HTTP_CONNECTION', 'HTTP_HOST', 'HTTP_USER_AGENT'
            ];
            
            foreach ($cgi_vars as $var) {
                $value = $_SERVER[$var] ?? 'Not set';
                echo "<tr><td>{$var}</td><td>" . htmlspecialchars($value) . "</td></tr>";
            }
            ?>
        </table>
    </div>

    <div class="container">
        <h2>📝 Form Processing Demo</h2>
        <form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
            <p>
                <label for="name">Name:</label>
                <input type="text" id="name" name="name" value="<?php echo htmlspecialchars($_POST['name'] ?? ''); ?>">
            </p>
            <p>
                <label for="message">Message:</label>
                <textarea id="message" name="message" rows="3" cols="50"><?php echo htmlspecialchars($_POST['message'] ?? ''); ?></textarea>
            </p>
            <p>
                <input type="submit" value="Submit via CGI">
            </p>
        </form>

        <?php if ($_SERVER['REQUEST_METHOD'] === 'POST' && !empty($_POST['name'])): ?>
        <div class="info">
            <strong>✅ Form Submitted Successfully!</strong><br>
            <strong>Name:</strong> <?php echo htmlspecialchars($_POST['name']); ?><br>
            <strong>Message:</strong> <?php echo htmlspecialchars($_POST['message']); ?><br>
            <strong>Submitted at:</strong> <?php echo date('Y-m-d H:i:s'); ?>
        </div>
        <?php endif; ?>
    </div>

    <div class="container">
        <h2>🔍 Raw POST Data</h2>
        <?php
        $raw_post = file_get_contents('php://input');
        if (!empty($raw_post)):
        ?>
        <div class="code">
            <strong>Raw POST Data:</strong><br>
            <?php echo htmlspecialchars($raw_post); ?>
        </div>
        <?php else: ?>
        <p><em>No POST data received. Submit the form above to see raw POST data.</em></p>
        <?php endif; ?>
    </div>

    <div class="container">
        <h2>⚡ Performance Test</h2>
        <?php
        $start_time = microtime(true);
        
        // Simulate some work
        $result = 0;
        for ($i = 0; $i < 100000; $i++) {
            $result += sqrt($i);
        }
        
        $end_time = microtime(true);
        $execution_time = ($end_time - $start_time) * 1000; // Convert to milliseconds
        ?>
        <p>✅ Computed square roots of numbers 0-99,999</p>
        <p><strong>Execution time:</strong> <?php echo number_format($execution_time, 2); ?> ms</p>
        <p><strong>Memory usage:</strong> <?php echo number_format(memory_get_usage() / 1024, 2); ?> KB</p>
        <p><strong>Peak memory:</strong> <?php echo number_format(memory_get_peak_usage() / 1024, 2); ?> KB</p>
    </div>

    <div class="container">
        <h2>📋 All Environment Variables</h2>
        <table>
            <tr><th>Variable</th><th>Value</th></tr>
            <?php
            ksort($_SERVER);
            foreach ($_SERVER as $key => $value) {
                if (is_string($value)) {
                    echo "<tr><td>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars($value) . "</td></tr>";
                }
            }
            ?>
        </table>
    </div>

    <footer style="text-align: center; margin-top: 40px; padding: 20px; color: #666;">
        <p>🔧 Generated by PHP CGI at <?php echo date('Y-m-d H:i:s'); ?></p>
        <p>This page demonstrates various CGI capabilities including environment variable access, form processing, and performance testing.</p>
    </footer>
</body>
</html>

