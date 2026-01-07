#!/bin/bash

echo "=== Stdlib Loading Diagnosis ==="
echo ""
echo "Current directory:"
pwd
echo ""
echo "Looking for stdlib directory:"
ls -la stdlib/ 2>/dev/null && echo "✓ Found ./stdlib" || echo "✗ No ./stdlib"
ls -la ../stdlib/ 2>/dev/null && echo "✓ Found ../stdlib" || echo "✗ No ../stdlib"
echo ""
echo "Stdlib files in ./stdlib:"
ls -la stdlib/*.mg 2>/dev/null | head -5
echo ""
echo "Checking if magolor compiler can find stdlib:"
echo "Try setting MAGOLOR_STDLIB environment variable:"
echo "  export MAGOLOR_STDLIB=$(pwd)/stdlib"
echo ""
echo "Or run from project root where stdlib/ exists"
