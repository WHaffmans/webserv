// Navigation functionality
document.addEventListener('DOMContentLoaded', function() {
    const hamburger = document.querySelector('.hamburger');
    const navMenu = document.querySelector('.nav-menu');
    const navLinks = document.querySelectorAll('.nav-link');

    // Toggle mobile menu
    hamburger.addEventListener('click', function() {
        hamburger.classList.toggle('active');
        navMenu.classList.toggle('active');
    });

    // Close mobile menu when clicking on a link
    navLinks.forEach(link => {
        link.addEventListener('click', function() {
            hamburger.classList.remove('active');
            navMenu.classList.remove('active');
        });
    });

    // Smooth scrolling for navigation links
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            const targetId = this.getAttribute('href');
            const targetSection = document.querySelector(targetId);
            
            if (targetSection) {
                const offsetTop = targetSection.offsetTop - 80;
                window.scrollTo({
                    top: offsetTop,
                    behavior: 'smooth'
                });
            }
        });
    });

    // Navbar background change on scroll
    window.addEventListener('scroll', function() {
        const navbar = document.querySelector('.navbar');
        if (window.scrollY > 100) {
            navbar.style.background = 'rgba(15, 23, 42, 0.98)';
        } else {
            navbar.style.background = 'rgba(15, 23, 42, 0.95)';
        }
    });

    // Intersection Observer for animations
    const observerOptions = {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
    };

    const observer = new IntersectionObserver(function(entries) {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.style.opacity = '1';
                entry.target.style.transform = 'translateY(0)';
            }
        });
    }, observerOptions);

    // Observe elements for animation
    const animatedElements = document.querySelectorAll('.feature-card, .docs-card, .stat-item');
    animatedElements.forEach(el => {
        el.style.opacity = '0';
        el.style.transform = 'translateY(30px)';
        el.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        observer.observe(el);
    });
});

// Demo functionality
async function testStatic() {
    const output = document.getElementById('demo-result');
    output.textContent = 'Testing static file serving...';
    
    try {
        // Simulate API call to test static files
        await new Promise(resolve => setTimeout(resolve, 1000));
        
        output.textContent = `HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234
Last-Modified: ${new Date().toUTCString()}
ETag: "abc123"

<!DOCTYPE html>
<html>
<head>
    <title>Static File Test</title>
</head>
<body>
    <h1>Successfully served static file!</h1>
    <p>WebServ efficiently handles static content delivery.</p>
</body>
</html>

✅ Static file served successfully
📊 Response time: 2.4ms
🔄 Keep-alive connection maintained`;
    } catch (error) {
        output.textContent = `❌ Error: ${error.message}`;
    }
}

async function testDirectory() {
    const output = document.getElementById('demo-result');
    output.textContent = 'Testing directory listing...';
    
    try {
        await new Promise(resolve => setTimeout(resolve, 800));
        
        output.textContent = `HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 2048

<!DOCTYPE html>
<html>
<head>
    <title>Directory Listing - /www/</title>
    <style>
        body { font-family: monospace; margin: 20px; }
        .file { margin: 5px 0; }
        .dir { color: #2563eb; }
        .size { color: #64748b; }
    </style>
</head>
<body>
    <h1>Index of /www/</h1>
    <hr>
    <div class="file">
        <span class="dir">📁 css/</span>
        <span class="size">4.0K</span>
    </div>
    <div class="file">
        <span class="dir">📁 js/</span>
        <span class="size">2.1K</span>
    </div>
    <div class="file">
        <span>📄 index.html</span>
        <span class="size">12.3K</span>
    </div>
    <div class="file">
        <span>📄 favicon.ico</span>
        <span class="size">1.2K</span>
    </div>
    <hr>
    <p>WebServ v1.0 - Directory listing enabled</p>
</body>
</html>

✅ Directory listing generated
📁 4 items found
🕒 Generated in 1.2ms`;
    } catch (error) {
        output.textContent = `❌ Error: ${error.message}`;
    }
}

async function testError() {
    const output = document.getElementById('demo-result');
    output.textContent = 'Testing error handling...';
    
    try {
        await new Promise(resolve => setTimeout(resolve, 600));
        
        output.textContent = `HTTP/1.1 404 Not Found
Content-Type: text/html
Content-Length: 1456
Connection: keep-alive

<!DOCTYPE html>
<html>
<head>
    <title>404 - Page Not Found</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, sans-serif;
            background: #0f172a;
            color: #f8fafc;
            text-align: center;
            padding: 50px;
        }
        .error-code { 
            font-size: 6rem; 
            color: #ef4444;
            font-weight: bold;
        }
        .error-message {
            font-size: 1.5rem;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <div class="error-code">404</div>
    <div class="error-message">Page Not Found</div>
    <p>The requested resource could not be found on this server.</p>
    <hr>
    <p><em>WebServ/1.0 Server</em></p>
</body>
</html>

🔍 Error Details:
- Request: GET /nonexistent.html
- Client IP: 192.168.1.100
- User-Agent: Mozilla/5.0...
- Timestamp: ${new Date().toISOString()}

✅ Custom error page served
🛡️ Error logged for security monitoring`;
    } catch (error) {
        output.textContent = `❌ Error: ${error.message}`;
    }
}

async function testHeaders() {
    const output = document.getElementById('demo-result');
    output.textContent = 'Testing HTTP headers...';
    
    try {
        await new Promise(resolve => setTimeout(resolve, 700));
        
        output.textContent = `Request Headers:
==================
GET /api/info HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (X11; Linux x86_64)
Accept: text/html,application/xhtml+xml,application/xml
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Cache-Control: no-cache

Response Headers:
=================
HTTP/1.1 200 OK
Server: WebServ/1.0
Date: ${new Date().toUTCString()}
Content-Type: application/json
Content-Length: 425
Connection: keep-alive
Cache-Control: public, max-age=3600
ETag: "v1.0-${Date.now()}"
X-Frame-Options: DENY
X-Content-Type-Options: nosniff
X-XSS-Protection: 1; mode=block

Response Body:
==============
{
  "server": "WebServ",
  "version": "1.0.0",
  "status": "running",
  "uptime": "2d 14h 32m",
  "connections": {
    "active": 127,
    "total": 45892
  },
  "performance": {
    "requests_per_second": 1542,
    "avg_response_time": "1.2ms",
    "memory_usage": "45.2MB"
  },
  "features": [
    "HTTP/1.1",
    "Keep-Alive",
    "Gzip Compression",
    "Virtual Hosts",
    "CGI Support"
  ]
}

✅ Headers processed successfully
🔒 Security headers applied
⚡ Response time: 1.8ms`;
    } catch (error) {
        output.textContent = `❌ Error: ${error.message}`;
    }
}

// Utility functions
function formatBytes(bytes, decimals = 2) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const dm = decimals < 0 ? 0 : decimals;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(dm)) + ' ' + sizes[i];
}

function getCurrentTimestamp() {
    return new Date().toISOString();
}

// Copy code functionality for code blocks
document.addEventListener('DOMContentLoaded', function() {
    const codeBlocks = document.querySelectorAll('pre code');
    
    codeBlocks.forEach(block => {
        const button = document.createElement('button');
        button.textContent = 'Copy';
        button.className = 'copy-btn';
        button.style.cssText = `
            position: absolute;
            top: 10px;
            right: 10px;
            background: var(--primary-color);
            color: white;
            border: none;
            padding: 5px 10px;
            border-radius: 4px;
            cursor: pointer;
            font-size: 12px;
            opacity: 0;
            transition: opacity 0.3s ease;
        `;
        
        const container = block.closest('.code-preview, pre');
        if (container) {
            container.style.position = 'relative';
            container.appendChild(button);
            
            container.addEventListener('mouseenter', () => {
                button.style.opacity = '1';
            });
            
            container.addEventListener('mouseleave', () => {
                button.style.opacity = '0';
            });
            
            button.addEventListener('click', async () => {
                try {
                    await navigator.clipboard.writeText(block.textContent);
                    button.textContent = 'Copied!';
                    setTimeout(() => {
                        button.textContent = 'Copy';
                    }, 2000);
                } catch (err) {
                    console.error('Failed to copy code:', err);
                }
            });
        }
    });
});

// Keyboard shortcuts
document.addEventListener('keydown', function(e) {
    // Ctrl/Cmd + K to focus search (if implemented)
    if ((e.ctrlKey || e.metaKey) && e.key === 'k') {
        e.preventDefault();
        // Focus search input if available
        const searchInput = document.querySelector('#search-input');
        if (searchInput) {
            searchInput.focus();
        }
    }
    
    // Escape to close mobile menu
    if (e.key === 'Escape') {
        const hamburger = document.querySelector('.hamburger');
        const navMenu = document.querySelector('.nav-menu');
        if (hamburger && navMenu) {
            hamburger.classList.remove('active');
            navMenu.classList.remove('active');
        }
    }
});

// Performance monitoring
if ('performance' in window) {
    window.addEventListener('load', function() {
        setTimeout(() => {
            const perfData = performance.getEntriesByType('navigation')[0];
            console.log('Page load performance:', {
                domContentLoaded: perfData.domContentLoadedEventEnd - perfData.domContentLoadedEventStart,
                loadComplete: perfData.loadEventEnd - perfData.loadEventStart,
                totalTime: perfData.loadEventEnd - perfData.fetchStart
            });
        }, 0);
    });
}
