#!/bin/bash
interval=1 # seconds between measurements
iterations=1000
output_file="client0.2_uss.csv"

# Create a proper CSV header
echo "Timestamp,Iteration,PID,Process,USS_KB" > $output_file

for ((i=1; i<=$iterations; i++)); do
  timestamp=$(date +"%Y-%m-%d %H:%M:%S")
  
  # Get only the first line with the actual client process data
  # Using grep to exclude summary lines, then getting the first match
  smem_output=$(smem -k -c "pid command uss" -P "^./client$" | grep -v "^---" | grep -v "^[[:space:]]*[0-9]\+[[:space:]]*$" | tail -n +2 | head -1)
  
  if [ -n "$smem_output" ]; then
    # Extract PID and USS separately, convert USS to KB
    pid=$(echo "$smem_output" | awk '{print $1}')
    uss_raw=$(echo "$smem_output" | awk '{print $NF}')
    
    # Handle different unit formats (K, M, G) and convert to KB
    if [[ $uss_raw == *K ]]; then
      uss_kb=$(echo $uss_raw | sed 's/K//')
    elif [[ $uss_raw == *M ]]; then
      uss_kb=$(echo $uss_raw | sed 's/M//' | awk '{print $1*1024}')
    elif [[ $uss_raw == *G ]]; then
      uss_kb=$(echo $uss_raw | sed 's/G//' | awk '{print $1*1024*1024}')
    else
      uss_kb=$uss_raw
    fi
    
    # Write properly formatted CSV line
    echo "$timestamp,$i,$pid,client,$uss_kb" >> $output_file
  else
    echo "$timestamp,$i,NA,client,NA" >> $output_file
  fi

done