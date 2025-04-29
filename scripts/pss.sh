#!/bin/bash
# Process to monitor (exact name)
CLIENT="client"
# File to write
FILE_NAME_CLIENT="pss_data_client.csv"

# Create or clear the output file with headers
echo "Timestamp,Iteration,PID,Process,PSS_KB" > "$FILE_NAME_CLIENT"

# Main monitoring loop
for i in {1..1000}; do
    TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
    ITERATION="$i"
    
    # Get PID of the exact client process
    PID=$(pgrep -x "$CLIENT")
    
    if [ -n "$PID" ]; then
        # Get PSS from /proc/[pid]/smaps
        if [ -f "/proc/$PID/smaps" ]; then
            PSS=$(grep -i pss /proc/$PID/smaps | awk '{total += $2} END {print total}')
            echo "$TIMESTAMP,$ITERATION,$PID,$CLIENT,$PSS" >> "$FILE_NAME_CLIENT"
        fi
    else
        echo "Process $CLIENT not found" >&2
    fi
    

done