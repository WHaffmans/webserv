// ------------------------------------------------------
// CODE EXECUTOR
// ------------------------------------------------------

const defaultCode = `print("Content-Type: text/plain\\n")
print("Hello from Python CGI!")
`;

document.getElementById("code-input").value = defaultCode;

document.getElementById("run-code").onclick = async () => {
    const code = document.getElementById("code-input").value;

    const resp = await fetch("/cgi-bin/python/execute.py", {
        method: "POST",
        body: new URLSearchParams({ code })
    });

    document.getElementById("code-output").textContent = await resp.text();
};

document.getElementById("reset-code").onclick = () => {
    document.getElementById("code-input").value = defaultCode;
    document.getElementById("code-output").textContent = "";
};


// ------------------------------------------------------
// FORM PROCESSOR
// ------------------------------------------------------

document.getElementById("form-processor").onsubmit = async (e) => {
    e.preventDefault();

    const formData = new FormData(e.target);

    const resp = await fetch("/cgi-bin/python/form.py", {
        method: "POST",
        body: formData
    });

    document.getElementById("form-output").textContent = await resp.text();
};


// ------------------------------------------------------
// CALCULATOR
// ------------------------------------------------------

let currentOp = "add";

document.querySelectorAll("#calc-ops button").forEach(btn => {
    btn.onclick = () => {
        currentOp = btn.dataset.op;
        document.querySelectorAll("#calc-ops button").forEach(b => b.classList.remove("bg-accent"));
        btn.classList.add("bg-accent");
    };
});

document.getElementById("calc-run").onclick = async () => {
    const a = document.getElementById("calc-a").value;
    const b = document.getElementById("calc-b").value;

    const resp = await fetch("/cgi-bin/python/calc.py", {
        method: "POST",
        body: new URLSearchParams({ a, b, op: currentOp })
    });

    document.getElementById("calc-output").textContent = await resp.text();
};


// ------------------------------------------------------
// SYSTEM INFO
// ------------------------------------------------------

async function loadSystemInfo() {
    const resp = await fetch("/cgi-bin/python/sysinfo.py");
    const json = await resp.json();

    const container = document.getElementById("sysinfo-grid");

    container.innerHTML = `
    <div class="grid grid-cols-2 sm:grid-cols-4 gap-4">

      <div class="p-3 bg-background border border-border rounded">
        <div class="text-xs text-gray-400 mb-1">Hostname</div>
        <div class="font-mono">${json.hostname}</div>
      </div>

      <div class="p-3 bg-background border border-border rounded">
        <div class="text-xs text-gray-400 mb-1">Platform</div>
        <div class="font-mono">${json.platform}</div>
      </div>

      <div class="p-3 bg-background border border-border rounded">
        <div class="text-xs text-gray-400 mb-1">Python</div>
        <div class="font-mono">${json.python_version}</div>
      </div>

      <div class="p-3 bg-background border border-border rounded">
        <div class="text-xs text-gray-400 mb-1">Processes</div>
        <div class="font-mono">${json.processes}</div>
      </div>

    </div>

    <div class="mt-4 text-xs text-gray-400 text-right font-mono">
      Updated: ${new Date(json.timestamp).toLocaleString()}
    </div>
  `;
}


document.getElementById("sys-refresh").onclick = loadSystemInfo;
loadSystemInfo();

document.getElementById("set-cookie").onclick = async () => {
    const resp = await fetch("/cgi-bin/python/cookie.py?set=hello123");
    document.getElementById("cookie-output").textContent = await resp.text();
};

document.getElementById("clear-cookie").onclick = async () => {
    const resp = await fetch("/cgi-bin/python/cookie.py?clear=1");
    document.getElementById("cookie-output").textContent = await resp.text();
};

async function loadSession() {
    const resp = await fetch("/cgi-bin/python/session.py");
    document.getElementById("session-output").textContent = await resp.text();
}

document.getElementById("load-session").onclick = loadSession;
document.getElementById("session-refresh").onclick = loadSession;

// Auto-load on page open:
loadSession();

