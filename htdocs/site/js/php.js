// -----------------------------
// Code Executor
// -----------------------------
const PHP_DEFAULT = `<?php
// PHP CGI Example
header("Content-Type: text/plain");

echo "Hello from PHP CGI!\\n";
echo "Current time: " . date("Y-m-d H:i:s") . "\\n";
echo "Server: Webserv\\n";
echo "PHP version: " . PHP_VERSION . "\\n";
?>`;

const EXAMPLES = {
    hello: `<?php header("Content-Type: text/plain"); echo "Hello, World from PHP CGI!"; ?>`,
    strings: `<?php
header("Content-Type: text/plain");
$str = "Hello, Webserv!";
echo "=== String Operations ===\\n";
echo "Original: " . $str . "\\n";
echo "Uppercase: " . strtoupper($str) . "\\n";
echo "Lowercase: " . strtolower($str) . "\\n";
echo "Length: " . strlen($str) . "\\n";
echo "Reversed: " . strrev($str) . "\\n"; ?>`,
    datetime: `<?php
header("Content-Type: text/plain");
echo "=== Date & Time ===\\n";
echo "Date: " . date("F j, Y") . "\\n";
echo "Time: " . date("g:i:s A") . "\\n";
echo "Timezone: " . date_default_timezone_get() . "\\n";
echo "Unix Timestamp: " . time() . "\\n";
echo "Day of week: " . date("l") . "\\n"; ?>`,
    server: `<?php
header("Content-Type: text/plain");
echo "=== Server Variables ===\\n";
echo "SERVER_SOFTWARE: " . $_SERVER['SERVER_SOFTWARE'] . "\\n";
echo "SERVER_NAME: " . $_SERVER['SERVER_NAME'] . "\\n";
echo "REQUEST_METHOD: " . $_SERVER['REQUEST_METHOD'] . "\\n";
echo "SCRIPT_NAME: " . $_SERVER['SCRIPT_NAME'] . "\\n";
echo "REMOTE_ADDR: " . $_SERVER['REMOTE_ADDR'] . "\\n"; ?>`,
};

document.getElementById("php-code-input").value = PHP_DEFAULT;

document.querySelectorAll(".php-example").forEach(btn => {
    btn.onclick = () => {
        document.getElementById("php-code-input").value = EXAMPLES[btn.dataset.example];
    };
});

document.getElementById("php-reset-code").onclick = () => {
    document.getElementById("php-code-input").value = PHP_DEFAULT;
    document.getElementById("php-code-output").textContent = "";
};

document.getElementById("php-run-code").onclick = async () => {
    const code = document.getElementById("php-code-input").value;
    const resp = await fetch("/cgi-bin/php/execute.php", {
        method: "POST",
        body: new URLSearchParams({ code })
    });
    document.getElementById("php-code-output").textContent = await resp.text();
};

// -----------------------------
// Form Processor
// -----------------------------
document.getElementById("php-form").onsubmit = async (e) => {
    e.preventDefault();
    const fd = new FormData(e.target);
    const resp = await fetch("/cgi-bin/php/form.php", { method: "POST", body: fd });
    document.getElementById("php-form-output").textContent = await resp.text();
};

// -----------------------------
// Session Demo
// -----------------------------
const sessionEmpty = document.getElementById("php-session-empty");
const sessionView = document.getElementById("php-session-view");
const sessionInfo = document.getElementById("php-session-info");
const sessionList = document.getElementById("php-session-list");
const btnStart = document.getElementById("php-session-start");
const btnDestroy = document.getElementById("php-session-destroy");
const btnAdd = document.getElementById("php-session-add");

async function refreshSession() {
    const resp = await fetch("/cgi-bin/php/session.php?action=get");
    const data = await resp.json();
    renderSession(data);
}

function renderSession(data) {
    if (!data.active) {
        sessionEmpty.classList.remove("hidden");
        sessionView.classList.add("hidden");
        btnDestroy.classList.add("hidden");
        return;
    }
    sessionEmpty.classList.add("hidden");
    sessionView.classList.remove("hidden");
    btnDestroy.classList.remove("hidden");

    sessionInfo.innerHTML = `
    <div class="flex justify-between"><span class="text-muted-foreground">session_id():</span><span class="text-[#777BB4]">${data.session_id}</span></div>
    <div class="flex justify-between"><span class="text-muted-foreground">REMOTE_ADDR:</span><span>${data.ip_address}</span></div>
    <div class="flex justify-between"><span class="text-muted-foreground">Created:</span><span>${data.created_at}</span></div>
  `;

    sessionList.innerHTML = "";
    Object.entries(data.data).forEach(([k, v]) => {
        const row = document.createElement("div");
        row.className = "flex items-center justify-between p-2 rounded-md bg-background border border-border";
        row.innerHTML = `
      <span class="font-mono text-sm">
        <span class="text-muted-foreground">$_SESSION['</span>
        <span class="text-[#777BB4]">${k}</span>
        <span class="text-muted-foreground">'] = '</span>
        <span>${v}</span>
        <span class="text-muted-foreground">';</span>
      </span>
      <button class="p-1 text-muted-foreground hover:text-red-400">🗑</button>
    `;
        row.querySelector("button").onclick = async () => {
            await fetch("/cgi-bin/php/session.php?action=remove", {
                method: "POST",
                body: new URLSearchParams({ key: k })
            });
            refreshSession();
        };
        sessionList.appendChild(row);
    });
}

btnStart.onclick = async () => {
    await fetch("/cgi-bin/php/session.php?action=start");
    refreshSession();
};

btnDestroy.onclick = async () => {
    await fetch("/cgi-bin/php/session.php?action=destroy");
    refreshSession();
};

btnAdd.onclick = async () => {
    const key = document.getElementById("php-s-key").value.trim();
    const value = document.getElementById("php-s-value").value;
    if (!key) return;
    await fetch("/cgi-bin/php/session.php?action=add", {
        method: "POST",
        body: new URLSearchParams({ key, value })
    });
    document.getElementById("php-s-key").value = "";
    document.getElementById("php-s-value").value = "";
    refreshSession();
};

refreshSession();

// -----------------------------
// Array Demo
// -----------------------------
const arrItems = document.getElementById("arr-items");
const arrCount = document.getElementById("arr-count");
const arrTotal = document.getElementById("arr-total");
const arrCode = document.getElementById("arr-code");
const arrPrint = document.getElementById("arr-printr");

async function arrayCall(action) {
    const resp = await fetch(`/cgi-bin/php/array.php?action=${encodeURIComponent(action)}`);
    const data = await resp.json();

    arrItems.innerHTML = "";
    data.array.forEach((item, idx) => {
        const div = document.createElement("div");
        div.className = "flex items-center justify-between p-3 rounded-lg bg-background border border-border";
        div.innerHTML = `
      <div class="flex items-center gap-3">
        <span class="w-6 h-6 flex items-center justify-center rounded bg-secondary text-xs font-mono text-muted-foreground">${idx}</span>
        <div>
          <span class="font-medium">${item.name}</span>
          <span class="ml-2 text-xs px-2 py-0.5 rounded-full bg-secondary text-muted-foreground">${item.category}</span>
        </div>
      </div>
      <span class="font-mono text-[#777BB4]">$${Number(item.price).toFixed(2)}</span>
    `;
        arrItems.appendChild(div);
    });

    arrCount.textContent = data.array.length;
    arrTotal.textContent = `$${Number(data.total).toFixed(2)}`;
    arrCode.textContent = data.code;
    arrPrint.textContent = data.printr;
}

document.querySelectorAll('[data-action]').forEach(btn => {
    btn.onclick = () => arrayCall(btn.dataset.action);
});

// initial load
arrayCall("reset");

// -----------------------------
// PHPINFO PANEL
// -----------------------------
document.getElementById("phpinfo-btn").onclick = async () => {
    const out = document.getElementById("phpinfo-content");
    out.innerHTML = `<div class="flex items-center gap-2 text-muted-foreground">
      <div class="w-4 h-4 border-2 border-[#777BB4] border-t-transparent rounded-full animate-spin"></div>
      <span>Loading phpinfo()…</span>
    </div>`;

    const resp = await fetch("/cgi-bin/php/phpinfo.php");
    const html = await resp.text();

    // Wrap phpinfo HTML in our styled container
    out.innerHTML = `
    <div class="phpinfo-output bg-background border border-border rounded p-4">
      ${html}
    </div>
  `;
};