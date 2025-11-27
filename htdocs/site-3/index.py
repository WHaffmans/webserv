#!/usr/bin/env python3

import os
import sys
from datetime import datetime
from urllib.parse import parse_qs

# Parse query string and POST data
query_params = parse_qs(os.environ.get('QUERY_STRING', ''))
form_data = {}
if os.environ.get('REQUEST_METHOD') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        form_data = parse_qs(post_data)

# Get or set visit counter cookie
cookies = os.environ.get('HTTP_COOKIE', '')
visit_count = 1
for cookie in cookies.split(';'):
    if cookie.strip().startswith('visit_count='):
        try:
            visit_count = int(cookie.split('=')[1]) + 1
        except ValueError:
            pass

# CGI Headers
print("Content-Type: text/html; charset=utf-8")
print(f"Set-Cookie: visit_count={visit_count}; Path=/; Max-Age=3600")
print("Set-Cookie: last_visit={}; Path=/".format(datetime.now().strftime('%Y-%m-%d_%H:%M:%S')))
print()  # End headers

# HTML Response
print(f"""<!DOCTYPE html>
<html>
<head>
    <title>Webserv Python CGI Capabilities</title>
    <style>
        body {{
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
            color: #0f0;
            padding: 20px;
            margin: 0;
        }}
        .container {{
            max-width: 900px;
            margin: 0 auto;
            background: rgba(0, 0, 0, 0.7);
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 0 20px rgba(0, 255, 0, 0.3);
        }}
        h1 {{
            color: #0ff;
            text-align: center;
            text-shadow: 0 0 10px #0ff;
        }}
        h2 {{
            color: #0f0;
            border-bottom: 2px solid #0f0;
            padding-bottom: 5px;
        }}
        .section {{
            margin: 20px 0;
            padding: 15px;
            background: rgba(0, 50, 0, 0.3);
            border-left: 3px solid #0f0;
        }}
        .key {{
            color: #ff0;
            font-weight: bold;
        }}
        .value {{
            color: #0ff;
        }}
        .highlight {{
            background: rgba(0, 255, 0, 0.1);
            padding: 2px 5px;
            border-radius: 3px;
        }}
        .snake {{
            text-align: center;
            font-size: 30px;
            margin: 20px 0;
            animation: wiggle 2s infinite;
        }}
        @keyframes wiggle {{
            0%, 100% {{ transform: translateX(0); }}
            25% {{ transform: translateX(-10px); }}
            75% {{ transform: translateX(10px); }}
        }}
        table {{
            width: 100%;
            border-collapse: collapse;
            margin: 10px 0;
        }}
        td {{
            padding: 8px;
            border-bottom: 1px solid #0f0;
        }}
        td:first-child {{
            width: 40%;
        }}
        form {{
            margin: 15px 0;
        }}
        input[type="text"] {{
            background: #000;
            color: #0f0;
            border: 2px solid #0f0;
            padding: 8px;
            font-family: 'Courier New', monospace;
            margin: 5px;
        }}
        button {{
            background: #0f0;
            color: #000;
            border: none;
            padding: 10px 20px;
            font-weight: bold;
            cursor: pointer;
            font-family: 'Courier New', monospace;
        }}
        button:hover {{
            background: #0ff;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="snake">🐍 Python CGI Demo 🐍</div>
        <h1>Webserv CGI Capabilities Showcase</h1>

        <div class="section">
            <h2>🍪 Cookie Management</h2>
            <p><span class="key">Visit Count:</span> <span class="value highlight">{visit_count}</span></p>
            <p><span class="key">Cookies Set:</span></p>
            <ul>
                <li>visit_count={visit_count} (expires in 1 hour)</li>
                <li>last_visit={datetime.now().strftime('%Y-%m-%d_%H:%M:%S')}</li>
            </ul>
            <p><span class="key">Received Cookies:</span> <span class="value">{cookies if cookies else 'None'}</span></p>
        </div>

        <div class="section">
            <h2>🌐 CGI Environment Variables</h2>
            <table>
                <tr><td class="key">GATEWAY_INTERFACE</td><td class="value">{os.environ.get('GATEWAY_INTERFACE', 'N/A')}</td></tr>
                <tr><td class="key">SERVER_PROTOCOL</td><td class="value">{os.environ.get('SERVER_PROTOCOL', 'N/A')}</td></tr>
                <tr><td class="key">REQUEST_METHOD</td><td class="value">{os.environ.get('REQUEST_METHOD', 'N/A')}</td></tr>
                <tr><td class="key">SCRIPT_NAME</td><td class="value">{os.environ.get('SCRIPT_NAME', 'N/A')}</td></tr>
                <tr><td class="key">PATH_INFO</td><td class="value">{os.environ.get('PATH_INFO', 'N/A')}</td></tr>
                <tr><td class="key">QUERY_STRING</td><td class="value">{os.environ.get('QUERY_STRING', 'N/A')}</td></tr>
                <tr><td class="key">REMOTE_ADDR</td><td class="value">{os.environ.get('REMOTE_ADDR', 'N/A')}</td></tr>
                <tr><td class="key">SERVER_NAME</td><td class="value">{os.environ.get('SERVER_NAME', 'N/A')}</td></tr>
                <tr><td class="key">SERVER_PORT</td><td class="value">{os.environ.get('SERVER_PORT', 'N/A')}</td></tr>
                <tr><td class="key">CONTENT_TYPE</td><td class="value">{os.environ.get('CONTENT_TYPE', 'N/A')}</td></tr>
                <tr><td class="key">CONTENT_LENGTH</td><td class="value">{os.environ.get('CONTENT_LENGTH', 'N/A')}</td></tr>
                <tr><td class="key">HTTP_USER_AGENT</td><td class="value">{os.environ.get('HTTP_USER_AGENT', 'N/A')}</td></tr>
                <tr><td class="key">HTTP_ACCEPT</td><td class="value">{os.environ.get('HTTP_ACCEPT', 'N/A')}</td></tr>
            </table>
        </div>

        <div class="section">
            <h2>📝 Form Handling (GET & POST)</h2>
            <form method="GET" action="/python">
                <input type="text" name="get_param" placeholder="Test GET parameter" value="{query_params.get('get_param', [''])[0]}">
                <button type="submit">Send GET</button>
            </form>
            <form method="POST" action="/python" enctype="application/x-www-form-urlencoded">
                <input type="text" name="post_param" placeholder="Test POST parameter">
                <button type="submit">Send POST</button>
            </form>
            {f'<p><span class="key">GET Parameters:</span> <span class="value">{dict(query_params)}</span></p>' if query_params else ''}
            {f'<p><span class="key">POST Data:</span> <span class="value">{dict(form_data)}</span></p>' if form_data else ''}
        </div>

        <div class="section">
            <h2>⚙️ Webserv Features Demonstrated</h2>
            <ul>
                <li>✅ <span class="key">CGI/1.1 Environment:</span> All standard CGI variables populated</li>
                <li>✅ <span class="key">Cookie Handling:</span> Set-Cookie headers and HTTP_COOKIE parsing</li>
                <li>✅ <span class="key">GET Requests:</span> QUERY_STRING parsing</li>
                <li>✅ <span class="key">POST Requests:</span> stdin reading with CONTENT_LENGTH</li>
                <li>✅ <span class="key">Response Headers:</span> Custom Content-Type and cookies</li>
                <li>✅ <span class="key">Process Execution:</span> fork/exec/pipe2 with non-blocking I/O</li>
                <li>✅ <span class="key">Dynamic Content:</span> Real-time HTML generation</li>
            </ul>
        </div>

        <div class="section">
            <h2>🐍 Python Script Info</h2>
            <p><span class="key">Python Version:</span> <span class="value">{sys.version}</span></p>
            <p><span class="key">Script Path:</span> <span class="value">{os.environ.get('SCRIPT_FILENAME', __file__)}</span></p>
            <p><span class="key">Working Directory:</span> <span class="value">{os.getcwd()}</span></p>
            <p><span class="key">Process ID:</span> <span class="value">{os.getpid()}</span></p>
        </div>
    </div>
</body>
</html>
""")

sys.stdout.flush()