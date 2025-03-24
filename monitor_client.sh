#!/bin/bash

# Processes to monitor
CLIENT="client"

# Files to write 
FILE_NAME_CLIENT="rss_data_client.csv"
# Create or clear the output file with headers
echo "Timestamp,PID,Process,RSS_KB" > "$FILE_NAME_CLIENT"

# Main monitoring loop
for i in {1..1000}; do
    TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
	ITERATION="$i"
    # Get RSS for specific process 
	ps -C "$CLIENT" -o pid,comm,rss | tail -n +2 | while read -r PID COMM RSS; do
        echo "$TIMESTAMP,$ITERATION,$PID,$COMM,$RSS" >> "$FILE_NAME_CLIENT"
	done 

done