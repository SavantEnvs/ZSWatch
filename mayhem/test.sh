#!/usr/bin/env bash
#
# mayhem/test.sh — RUN this repo's OWN functional test suite (already built by mayhem/build.sh).
# exit 0 = pass. EDIT per repo. PATCH-grade oracle: after an agent patches the source, the grader
# rebuilds (build.sh) then runs this. DELETE this file if the repo has no meaningful tests.
#
# IMPORTANT:
#  * Must assert BEHAVIOR/OUTPUT, not just exit status. The oracle has to check asserted values /
#    golden-output diffs / known-answer results — so a PATCH that "fixes" a bug by making the program
#    exit(0) (or any no-op) FAILS here. Running inputs and checking only "exit 0 / didn't crash" is
#    NOT a functional test (it's trivially reward-hackable) — use the project's real assertion suite.
#  * Do NOT build here — mayhem/build.sh already compiled the test suite (with the project's normal
#    flags). This script only RUNS the pre-built tests and reports counts. If the test runner is
#    missing, that's a build.sh bug — fail loudly rather than silently rebuilding.
#  * REQUIRED OUTPUT — a CTRF (https://ctrf.io) summary so Mayhem/the PATCH grader reads the counts:
#      - writes a CTRF JSON report to ${CTRF_REPORT:-$SRC/ctrf-report.json}, and
#      - prints a one-line `CTRF {...}` marker to stdout (same JSON, compact).
#    Only `results.summary` (with tests/passed/failed/pending/skipped/other) is required.
#    Use the emit_ctrf helper below; it computes tests = passed+failed+skipped and sets the exit
#    code (0 iff failed==0). Map your framework's output to passed/failed/skipped.
set -uo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${MAYHEM_JOBS:=$(nproc)}"   # build parallelism; env-overridable, falls back to nproc (use -j"$MAYHEM_JOBS")
cd "$SRC"

# emit_ctrf <tool> <passed> <failed> [skipped] [pending] [other]
# Writes a CTRF report (file + stdout `CTRF {...}` marker) and returns non-zero iff failed>0.
emit_ctrf() {
  local tool="$1" passed="$2" failed="$3" skipped="${4:-0}" pending="${5:-0}" other="${6:-0}"
  local tests=$(( passed + failed + skipped + pending + other ))
  cat > "${CTRF_REPORT:-$SRC/ctrf-report.json}" <<JSON
{
  "results": {
    "tool": { "name": "$tool" },
    "summary": {
      "tests": $tests,
      "passed": $passed,
      "failed": $failed,
      "pending": $pending,
      "skipped": $skipped,
      "other": $other
    }
  }
}
JSON
  printf 'CTRF {"results":{"tool":{"name":"%s"},"summary":{"tests":%d,"passed":%d,"failed":%d,"pending":%d,"skipped":%d,"other":%d}}}\n' \
    "$tool" "$tests" "$passed" "$failed" "$pending" "$skipped" "$other"
  [ "$failed" -eq 0 ]
}

# Known-answer probes: feed fixed BLE messages to the real parsers (selftest_* binaries, built by build.sh
# with normal flags) and compare, here in bash, what they emitted with the expected event.
PASSED=0 FAILED=0
hx() { printf '%s' "$1" | od -An -tx1 | tr -d ' \n'; }
expect() {  # expect <name> <expected> <actual>
  if [ "$2" == "$3" ]; then PASSED=$((PASSED + 1)); echo "PASS $1"
  else FAILED=$((FAILED + 1)); echo "FAIL $1"; echo "  expected: $2"; echo "  actual:   $3"; fi
}
probe_gb() { "$SRC/selftest_gadgetbridge" "$(hx "$1")" 2>&1 | tr '\n' '|'; }
probe_ch() { "$SRC/selftest_chronos" "$1" 2>&1 | tr '\n' '|'; }

for b in selftest_gadgetbridge selftest_chronos; do
  [ -x "$SRC/$b" ] || { echo "missing $SRC/$b (mayhem/build.sh must build it)"; FAILED=$((FAILED + 1)); }
done

expect gb-notify 'events=1 sent=0|type=notify id=1234 src=Gmail title=Hello body=World|' \
  "$(probe_gb 'GB({"t":"notify","id":1234,"src":"Gmail","title":"Hello","body":"World"})')"
expect gb-notify-atob 'events=1 sent=0|type=notify id=5 src=Signal title=Hello body=World|' \
  "$(probe_gb 'GB({"t":"notify","id":5,"src":"Signal","title":atob("SGVsbG8="),"body":atob("V29ybGQ=")})')"
expect gb-notify-remove 'events=1 sent=0|type=notify_remove id=77|' \
  "$(probe_gb 'GB({"t":"notify-","id":77})')"
expect gb-musicstate 'events=1 sent=0|type=music_state playing=1 position=42 shuffle=1 repeat=2|' \
  "$(probe_gb 'GB({"t":"musicstate","state":"play","position":42,"shuffle":1,"repeat":2})')"
expect gb-musicinfo 'events=1 sent=0|type=music_info artist=A album=B track=C duration=200|' \
  "$(probe_gb 'GB({"t":"musicinfo","artist":"A","album":"B","track":"C","dur":200})')"
expect gb-weather 'events=1 sent=0|type=weather temp=-5 hum=97 wind=2 code=802 text=cloudy|' \
  "$(probe_gb 'GB({"t":"weather","temp":268,"hum":97,"code":802,"txt":"cloudy","wind":2.0,"wdir":14,"loc":"MALMO"})')"
expect gb-gps 'events=1 sent=0|type=gps lat=55.600 lon=13.000 sats=7|' \
  "$(probe_gb 'GB({"t":"gps","lat":55.6,"lon":13.0,"alt":10,"speed":1,"time":1700000000,"satellites":7,"hdop":1.5})')"
expect gb-settime 'events=1 sent=0|type=set_time seconds=1700000000|' "$(probe_gb 'setTime(1700000000);')"
expect gb-no-gb-frame 'events=0 sent=0|' "$(probe_gb '{"t":"notify","id":1}')"
expect gb-unknown-type 'events=0 sent=0|' "$(probe_gb 'GB({"t":"nonsense","id":1})')"

# Chronos: AB <len-3 BE16> FF <cmd> 00 <payload>; 0x93 sets the time, 0x72 is a notification.
expect ch-time 'events=1 sent=0|type=set_time seconds=1742041845|' "$(probe_ch ab000bff93000007e9030f0c1e2d)"
expect ch-notify 'events=1 sent=0|type=notify src=Gmail title=Title body=Body|' \
  "$(probe_ch "ab000fff72000b02$(hx 'Title:Body')" | sed 's/ id=[0-9]*//')"
expect ch-notify-no-title 'events=1 sent=0|type=notify src=Mail title=Mail body=Just text|' \
  "$(probe_ch "ab000eff72000402$(hx 'Just text')" | sed 's/ id=[0-9]*//')"
expect ch-truncated-packet 'events=0 sent=0|' "$(probe_ch ab000bff930000)"
expect ch-bad-magic 'events=0 sent=0|' "$(probe_ch 11000bff93000007e9030f0c1e2d)"

emit_ctrf "zswatch-ble-parsers" "$PASSED" "$FAILED" 0
