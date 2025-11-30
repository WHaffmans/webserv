#!/usr/bin/env python3
import os
import shutil
import platform
import psutil
import json
from datetime import datetime

print("Content-Type: application/json\r\n")

info = {
    "hostname": platform.node(),
    "platform": platform.platform(),
    "python_version": platform.python_version(),
    "uptime": f"{int(psutil.boot_time())}",
    "cpu_count": os.cpu_count(),
    "memory_percent": psutil.virtual_memory().percent,
    "disk_percent": psutil.disk_usage('/').percent,
    "load_average": ", ".join([str(x) for x in os.getloadavg()]),
    "processes": len(psutil.pids()),
    "timestamp": datetime.now().isoformat()
}

print(json.dumps(info))
