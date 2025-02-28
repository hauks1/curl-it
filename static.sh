#!/bin/bash

BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 binary_file"
    exit 1
fi

# Get section sizes
TEXT_SIZE=$(size -A -d "$BINARY" | grep '\.text' | awk '{print $2}')
RODATA_SIZE=$(size -A -d "$BINARY" | grep '\.rodata' | awk '{print $2}')
DATA_SIZE=$(size -A -d "$BINARY" | grep '\.data' | awk '{print $2}')
BSS_SIZE=$(size -A -d "$BINARY" | grep '\.bss' | awk '{print $2}')

# Calculate stack usage (if .su files exist)
STACK_SIZE=0
if [ -f "${BINARY%.o}.su" ]; then
    STACK_SIZE=$(awk '{sum += $2} END {print sum}' "${BINARY%.o}.su")
else
    echo "No stack usage file found. Compile with -fstack-usage to get stack information."
fi

# Calculate totals
ROM_SIZE=$((TEXT_SIZE + RODATA_SIZE))
RAM_SIZE=$((DATA_SIZE + BSS_SIZE + STACK_SIZE))

echo "Memory Usage Analysis for $BINARY:"
echo "-------------------------------------"
echo "ROM Usage:"
echo "  .text:   $TEXT_SIZE bytes"
echo "  .rodata: $RODATA_SIZE bytes"
echo "  Total:   $ROM_SIZE bytes"
echo
echo "RAM Usage:"
echo "  .data:   $DATA_SIZE bytes"
echo "  .bss:    $BSS_SIZE bytes"
echo "  Stack:   $STACK_SIZE bytes"
echo "  Total:   $RAM_SIZE bytes"