#!/bin/bash
# Debug Magolor compiler hanging during file compilation

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}=================================="
echo "MAGOLOR COMPILER HANG DEBUGGER"
echo "==================================${NC}"
echo ""

FILE="src/Slate/DB.mg"

if [ ! -f "$FILE" ]; then
    echo -e "${RED}ERROR: $FILE not found${NC}"
    exit 1
fi

echo -e "${CYAN}[1] Analyzing $FILE...${NC}"
echo "  Size: $(wc -c < $FILE) bytes"
echo "  Lines: $(wc -l < $FILE) lines"
echo ""

echo -e "${CYAN}[2] File contents:${NC}"
echo "---START---"
cat -n "$FILE"
echo "---END---"
echo ""

echo -e "${CYAN}[3] Looking for potential infinite loop triggers...${NC}"

# Check for circular imports
echo "  Imports/uses:"
grep -n "^use " "$FILE" || echo "    (none)"
grep -n "^import " "$FILE" || echo "    (none)"

# Check for recursive structures
echo ""
echo "  Classes:"
grep -n "^class " "$FILE" || echo "    (none)"

echo ""
echo "  Functions:"
grep -n "^fn " "$FILE" || echo "    (none)"

echo ""
echo "  Type definitions:"
grep -n "^type " "$FILE" || echo "    (none)"

echo ""
echo -e "${CYAN}[4] Testing with smaller sections...${NC}"

# Try compiling with empty file first
echo "  Test 1: Empty file"
echo "" > /tmp/test_empty.mg
timeout 5 magolor compile /tmp/test_empty.mg 2>&1 || echo "    Failed/timeout"

# Try with just imports
echo ""
echo "  Test 2: Just imports from your file"
grep "^use \|^import " "$FILE" > /tmp/test_imports.mg || echo "" > /tmp/test_imports.mg
if [ -s /tmp/test_imports.mg ]; then
    cat /tmp/test_imports.mg
    timeout 10 magolor compile /tmp/test_imports.mg 2>&1 || echo "    Failed/timeout on imports!"
else
    echo "    (no imports to test)"
fi

# Try with first 10 lines
echo ""
echo "  Test 3: First 10 lines"
head -10 "$FILE" > /tmp/test_10lines.mg
timeout 10 magolor compile /tmp/test_10lines.mg 2>&1 || echo "    Failed/timeout"

# Try with first 20 lines
echo ""
echo "  Test 4: First 20 lines"
head -20 "$FILE" > /tmp/test_20lines.mg
timeout 10 magolor compile /tmp/test_20lines.mg 2>&1 || echo "    Failed/timeout"

# Try each section incrementally
echo ""
echo "  Test 5: Binary search for problematic section"
TOTAL_LINES=$(wc -l < "$FILE")
SECTION_SIZE=5

for START in $(seq 1 $SECTION_SIZE $TOTAL_LINES); do
    END=$((START + SECTION_SIZE - 1))
    echo -n "    Lines $START-$END: "
    
    sed -n "${START},${END}p" "$FILE" > /tmp/test_section.mg
    
    if timeout 5 magolor compile /tmp/test_section.mg > /dev/null 2>&1; then
        echo -e "${GREEN}OK${NC}"
    else
        echo -e "${RED}HANG/FAIL${NC}"
        echo "      Problematic section:"
        sed -n "${START},${END}p" "$FILE" | cat -n
        break
    fi
done

echo ""
echo -e "${CYAN}[5] Testing compilation with strace (system call trace)${NC}"
echo "  This will show what the compiler is doing when it hangs..."
echo "  Running: timeout 30 strace -o /tmp/magolor.strace magolor compile $FILE"
echo ""

timeout 30 strace -o /tmp/magolor.strace magolor compile "$FILE" 2>&1 &
STRACE_PID=$!

# Monitor for 10 seconds
for i in {1..10}; do
    echo -n "."
    sleep 1
    if ! kill -0 $STRACE_PID 2>/dev/null; then
        break
    fi
done
echo ""

# Kill if still running
if kill -0 $STRACE_PID 2>/dev/null; then
    echo "  Compiler still running after 10s - killing..."
    kill $STRACE_PID 2>/dev/null || true
    wait $STRACE_PID 2>/dev/null || true
fi

echo ""
echo "  Last 50 system calls before hang:"
tail -50 /tmp/magolor.strace

echo ""
echo "  Looking for infinite loops (repeated calls):"
tail -500 /tmp/magolor.strace | sort | uniq -c | sort -rn | head -20

echo ""
echo -e "${CYAN}[6] Memory usage check${NC}"
echo "  If compiler is leaking memory, that's a sign of infinite recursion"
echo "  Running compilation with memory monitoring..."

(
    timeout 20 magolor compile "$FILE" 2>&1 &
    COMPILE_PID=$!
    
    echo "  PID: $COMPILE_PID"
    sleep 2
    
    for i in {1..10}; do
        if ! kill -0 $COMPILE_PID 2>/dev/null; then
            echo "  Process finished"
            break
        fi
        
        # Get memory usage
        if [ -f "/proc/$COMPILE_PID/status" ]; then
            MEM=$(grep VmRSS /proc/$COMPILE_PID/status | awk '{print $2}')
            echo "  Memory at ${i}s: ${MEM} kB"
        fi
        
        sleep 2
    done
    
    # Kill if still running
    kill $COMPILE_PID 2>/dev/null || true
) || true

echo ""
echo -e "${GREEN}=================================="
echo "DEBUGGING COMPLETE"
echo "==================================${NC}"
echo ""
echo "Summary of findings above. Look for:"
echo "  1. Which test case first causes a hang"
echo "  2. Repeated system calls (infinite loop)"
echo "  3. Growing memory usage (memory leak/recursion)"
echo "  4. Specific lines that trigger the hang"
