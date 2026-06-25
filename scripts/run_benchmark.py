import json
import urllib.error
import urllib.request
from pathlib import Path

script_path = Path(__file__).with_name("bench_10000.sql")
if not script_path.exists():
    raise SystemExit("Run: python3 scripts/generate_benchmark.py")
body = json.dumps({"script": script_path.read_text(encoding="utf-8")}).encode("utf-8")
request = urllib.request.Request(
    "http://localhost:8080/query",
    data=body,
    headers={"Content-Type": "application/json"},
    method="POST",
)
try:
    response = urllib.request.urlopen(request, timeout=180)
except urllib.error.HTTPError as error:
    response = error
result = json.loads(response.read().decode("utf-8"))
Path(__file__).with_name("bench_10000_result.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
for item in result.get("results", []):
    if item.get("statement", "").upper().startswith("SELECT"):
        print(item["message"])
        print(f"Time: {item['elapsedMs']:.3f} ms\n")
