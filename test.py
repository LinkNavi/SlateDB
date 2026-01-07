#!/usr/bin/env python3
"""
Monitor Magolor compiler for hangs during file compilation
"""

import subprocess
import time
import signal
import sys
import os
import psutil
from pathlib import Path

def monitor_compilation(file_path, timeout=30):
    """Monitor Magolor compilation with detailed diagnostics"""
    
    print("="*70)
    print("MAGOLOR COMPILATION MONITOR")
    print("="*70)
    print(f"File: {file_path}")
    print(f"Size: {os.path.getsize(file_path):,} bytes")
    print(f"Lines: {sum(1 for _ in open(file_path)):,}")
    print("="*70)
    print()
    
    # Show file contents
    print("[FILE CONTENTS]")
    with open(file_path) as f:
        for i, line in enumerate(f, 1):
            print(f"{i:4d} | {line.rstrip()}")
    print()
    
    # Start compilation
    cmd = ["magolor", "compile", file_path]
    print(f"Running: {' '.join(cmd)}")
    print()
    
    try:
        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True
        )
        
        start_time = time.time()
        last_output_time = start_time
        
        print(f"{'Time':<8} {'CPU%':<8} {'Memory':<12} {'Threads':<10} {'Status':<20}")
        print("-" * 70)
        
        while True:
            elapsed = time.time() - start_time
            
            # Check if process is still running
            if process.poll() is not None:
                print(f"\n✅ Process completed in {elapsed:.1f}s")
                stdout, stderr = process.communicate()
                if stdout:
                    print("\n[STDOUT]")
                    print(stdout)
                if stderr:
                    print("\n[STDERR]")
                    print(stderr)
                return True
            
            # Timeout check
            if elapsed > timeout:
                print(f"\n⚠️  TIMEOUT after {timeout}s - killing process")
                process.terminate()
                time.sleep(2)
                if process.poll() is None:
                    process.kill()
                    
                stdout, stderr = process.communicate()
                print("\n[PARTIAL OUTPUT]")
                if stdout:
                    print(stdout)
                if stderr:
                    print(stderr)
                    
                return False
            
            # Get process info
            try:
                proc_info = psutil.Process(process.pid)
                cpu_percent = proc_info.cpu_percent(interval=0.5)
                memory_mb = proc_info.memory_info().rss / 1024 / 1024
                num_threads = proc_info.num_threads()
                
                # Detect issues
                status = "Compiling..."
                if cpu_percent < 1.0 and elapsed > 3:
                    status = "⚠️  VERY LOW CPU"
                elif cpu_percent > 99:
                    status = "🔥 CPU MAXED OUT"
                elif memory_mb > 500:
                    status = "⚠️  HIGH MEMORY"
                
                # Check for child processes
                children = proc_info.children(recursive=True)
                if children:
                    status += f" (+{len(children)} children)"
                
                print(f"{elapsed:>6.1f}s  {cpu_percent:>6.1f}%  {memory_mb:>8.1f} MB  "
                      f"{num_threads:>3d}       {status}")
                
                # Check if stuck (very low CPU for >5 seconds)
                if cpu_percent < 1.0 and elapsed > 5:
                    print("\n🚨 DETECTED: Process appears stuck (very low CPU)")
                    print("   Possible causes:")
                    print("   - Infinite loop in compiler")
                    print("   - Waiting on I/O that will never complete")
                    print("   - Deadlock in parser/type checker")
                    
                    # Try to get backtrace with gdb
                    print("\n   Attempting to get stack trace with gdb...")
                    try:
                        gdb_cmd = f"gdb -batch -ex 'thread apply all bt' -p {process.pid}"
                        backtrace = subprocess.check_output(
                            gdb_cmd, 
                            shell=True, 
                            stderr=subprocess.STDOUT,
                            timeout=5
                        ).decode()
                        print("\n[STACK TRACE]")
                        print(backtrace)
                    except:
                        print("   (gdb not available or failed)")
                    
                    print("\n   Killing process...")
                    process.terminate()
                    return False
                    
            except psutil.NoSuchProcess:
                print("\n❌ Process disappeared")
                return False
            except Exception as e:
                print(f"\n❌ Monitoring error: {e}")
                return False
            
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n\n⚠️  Interrupted by user")
        process.terminate()
        return False

def test_progressive_sections(file_path):
    """Test compilation of progressively larger sections of the file"""
    
    print("\n" + "="*70)
    print("PROGRESSIVE SECTION TESTING")
    print("="*70)
    print("Testing compilation of progressively larger sections...")
    print()
    
    with open(file_path) as f:
        lines = f.readlines()
    
    total_lines = len(lines)
    
    # Test sections: 5, 10, 20, 40, ... lines
    test_sizes = [5, 10, 20, 40, 80, 160, total_lines]
    test_sizes = [s for s in test_sizes if s <= total_lines]
    
    for size in test_sizes:
        print(f"Testing first {size} lines... ", end="", flush=True)
        
        # Create temp file with first N lines
        temp_file = "/tmp/test_section.mg"
        with open(temp_file, 'w') as f:
            f.writelines(lines[:size])
        
        # Try to compile with short timeout
        try:
            result = subprocess.run(
                ["magolor", "compile", temp_file],
                capture_output=True,
                timeout=10,
                text=True
            )
            
            if result.returncode == 0:
                print("✅ OK")
            else:
                print(f"❌ FAILED")
                if result.stderr:
                    print(f"   Error: {result.stderr[:200]}")
        except subprocess.TimeoutExpired:
            print(f"⏱️  TIMEOUT (hang)")
            print(f"\n🚨 FOUND IT! The hang occurs somewhere in lines 1-{size}")
            
            if size > 5:
                prev_size = test_sizes[test_sizes.index(size) - 1]
                print(f"   Last working section: {prev_size} lines")
                print(f"   Problem is likely in lines {prev_size+1}-{size}")
                print()
                print("[PROBLEMATIC SECTION]")
                for i in range(prev_size, size):
                    print(f"{i+1:4d} | {lines[i].rstrip()}")
            
            return size
    
    print("\n✅ All sections compiled successfully")
    return None

def analyze_imports(file_path):
    """Analyze imports that might cause circular dependencies"""
    
    print("\n" + "="*70)
    print("IMPORT ANALYSIS")
    print("="*70)
    
    with open(file_path) as f:
        lines = f.readlines()
    
    imports = []
    uses = []
    
    for i, line in enumerate(lines, 1):
        line = line.strip()
        if line.startswith("import "):
            imports.append((i, line))
        elif line.startswith("use "):
            uses.append((i, line))
    
    if imports:
        print("\nImports:")
        for line_num, line in imports:
            print(f"  Line {line_num}: {line}")
    else:
        print("\nNo imports found")
    
    if uses:
        print("\nUses:")
        for line_num, line in uses:
            print(f"  Line {line_num}: {line}")
    else:
        print("\nNo uses found")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 debug_magolor_hang.py <file.mg>")
        sys.exit(1)
    
    file_path = sys.argv[1]
    
    if not os.path.exists(file_path):
        print(f"❌ File not found: {file_path}")
        sys.exit(1)
    
    # Analyze imports first
    analyze_imports(file_path)
    
    # Test progressive sections
    hang_line = test_progressive_sections(file_path)
    
    if hang_line:
        print("\n⚠️  Hang detected - skipping full compilation test")
        sys.exit(1)
    
    # Full compilation with monitoring
    print("\n" + "="*70)
    print("FULL COMPILATION TEST")
    print("="*70)
    
    success = monitor_compilation(file_path, timeout=30)
    
    if success:
        print("\n✅ Compilation completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Compilation failed or timed out")
        sys.exit(1)

if __name__ == "__main__":
    main()
