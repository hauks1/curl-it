#!/bin/bash
# save as extract_memory_usage.sh

# Check if a file was provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <pmap_output_file>"
    exit 1
fi

PMAP_FILE=$1

# Extract total RSS and PSS from the last line
TOTAL_RSS=$(tail -1 "$PMAP_FILE" | awk '{print $7}')
TOTAL_PSS=$(tail -1 "$PMAP_FILE" | awk '{print $8}')

# Set defaults if empty or invalid
[ -z "$TOTAL_RSS" ] || [ "$TOTAL_RSS" -lt 1 ] && TOTAL_RSS=$(grep -v "===" "$PMAP_FILE" | awk '{sum += $7} END {print sum}')
[ -z "$TOTAL_PSS" ] || [ "$TOTAL_PSS" -lt 1 ] && TOTAL_PSS=$(grep -v "===" "$PMAP_FILE" | awk '{sum += $8} END {print sum}')

# Extract application heap (both RSS and PSS)
HEAP_RSS=$(grep "\[heap\]" "$PMAP_FILE" | awk '{print $7}')
HEAP_PSS=$(grep "\[heap\]" "$PMAP_FILE" | awk '{print $8}')
[ -z "$HEAP_RSS" ] && HEAP_RSS=0
[ -z "$HEAP_PSS" ] && HEAP_PSS=0

# Extract RSS and PSS for key libraries
extract_mem() {
    local rss=$(grep "$1" "$PMAP_FILE" | awk '{sum += $7} END {print sum}')
    local pss=$(grep "$1" "$PMAP_FILE" | awk '{sum += $8} END {print sum}')
    [ -z "$rss" ] && rss=0
    [ -z "$pss" ] && pss=0
    echo "$rss $pss"
}

RELIC_MEM=$(extract_mem "librelic.so")
RELIC_RSS=$(echo $RELIC_MEM | cut -d' ' -f1)
RELIC_PSS=$(echo $RELIC_MEM | cut -d' ' -f2)

CRYPTO_MEM=$(extract_mem "libcrypto.so")
CRYPTO_RSS=$(echo $CRYPTO_MEM | cut -d' ' -f1)
CRYPTO_PSS=$(echo $CRYPTO_MEM | cut -d' ' -f2)

GNUTLS_MEM=$(extract_mem "libgnutls.so")
GNUTLS_RSS=$(echo $GNUTLS_MEM | cut -d' ' -f1)
GNUTLS_PSS=$(echo $GNUTLS_MEM | cut -d' ' -f2)

CURL_MEM=$(extract_mem "libcurl.so")
CURL_RSS=$(echo $CURL_MEM | cut -d' ' -f1)
CURL_PSS=$(echo $CURL_MEM | cut -d' ' -f2)

CJSON_MEM=$(extract_mem "libcjson.so")
CJSON_RSS=$(echo $CJSON_MEM | cut -d' ' -f1)
CJSON_PSS=$(echo $CJSON_MEM | cut -d' ' -f2)

LIBC_MEM=$(extract_mem "libc-2.31.so")
LIBC_RSS=$(echo $LIBC_MEM | cut -d' ' -f1)
LIBC_PSS=$(echo $LIBC_MEM | cut -d' ' -f2)

# Calculate cryptographic total
CRYPTO_TOTAL_RSS=$((RELIC_RSS + CRYPTO_RSS + GNUTLS_RSS))
CRYPTO_TOTAL_PSS=$((RELIC_PSS + CRYPTO_PSS + GNUTLS_PSS))

# Calculate directly measured components
MEASURED_TOTAL_RSS=$((HEAP_RSS + CRYPTO_TOTAL_RSS + CURL_RSS + CJSON_RSS + LIBC_RSS))
MEASURED_TOTAL_PSS=$((HEAP_PSS + CRYPTO_TOTAL_PSS + CURL_PSS + CJSON_PSS + LIBC_PSS))

# Calculate other system libraries (ensuring it's never negative)
OTHER_LIBS_RSS=$((TOTAL_RSS - MEASURED_TOTAL_RSS))
[ "$OTHER_LIBS_RSS" -lt 0 ] && OTHER_LIBS_RSS=0

OTHER_LIBS_PSS=$((TOTAL_PSS - MEASURED_TOTAL_PSS))
[ "$OTHER_LIBS_PSS" -lt 0 ] && OTHER_LIBS_PSS=0

# Calculate total system libraries
SYSTEM_LIBS_RSS=$((LIBC_RSS + OTHER_LIBS_RSS))
SYSTEM_LIBS_PSS=$((LIBC_PSS + OTHER_LIBS_PSS))

# Calculate percentages safely (avoiding division by zero)
calc_percent_rss() {
    if [ "$TOTAL_RSS" -eq 0 ]; then
        echo "0.0"
    else
        echo "scale=1; $1 * 100 / $TOTAL_RSS" | bc 2>/dev/null || echo "0.0"
    fi
}

calc_percent_pss() {
    if [ "$TOTAL_PSS" -eq 0 ]; then
        echo "0.0"
    else
        echo "scale=1; $1 * 100 / $TOTAL_PSS" | bc 2>/dev/null || echo "0.0"
    fi
}

HEAP_PERCENT_RSS=$(calc_percent_rss $HEAP_RSS)
CRYPTO_PERCENT_RSS=$(calc_percent_rss $CRYPTO_TOTAL_RSS)
CURL_PERCENT_RSS=$(calc_percent_rss $CURL_RSS)
CJSON_PERCENT_RSS=$(calc_percent_rss $CJSON_RSS)
SYSTEM_PERCENT_RSS=$(calc_percent_rss $SYSTEM_LIBS_RSS)

HEAP_PERCENT_PSS=$(calc_percent_pss $HEAP_PSS)
CRYPTO_PERCENT_PSS=$(calc_percent_pss $CRYPTO_TOTAL_PSS)
CURL_PERCENT_PSS=$(calc_percent_pss $CURL_PSS)
CJSON_PERCENT_PSS=$(calc_percent_pss $CJSON_PSS)
SYSTEM_PERCENT_PSS=$(calc_percent_pss $SYSTEM_LIBS_PSS)

# Output JSON format
cat << EOF
{
  "total": {
    "rss": $TOTAL_RSS,
    "pss": $TOTAL_PSS,
    "units": "KB"
  },
  "breakdown": {
    "application": {
      "heap": {
        "rss": $HEAP_RSS,
        "pss": $HEAP_PSS,
        "units": "KB"
      }
    },
    "libraries": {
      "cryptographic": {
        "librelic.so": {
          "rss": $RELIC_RSS,
          "pss": $RELIC_PSS
        },
        "libcrypto.so": {
          "rss": $CRYPTO_RSS,
          "pss": $CRYPTO_PSS
        },
        "libgnutls.so": {
          "rss": $GNUTLS_RSS,
          "pss": $GNUTLS_PSS
        },
        "total": {
          "rss": $CRYPTO_TOTAL_RSS,
          "pss": $CRYPTO_TOTAL_PSS
        },
        "units": "KB"
      },
      "networking": {
        "libcurl.so": {
          "rss": $CURL_RSS,
          "pss": $CURL_PSS
        },
        "units": "KB"
      },
      "data_processing": {
        "libcjson.so": {
          "rss": $CJSON_RSS,
          "pss": $CJSON_PSS
        },
        "units": "KB"
      },
      "system": {
        "libc-2.31.so": {
          "rss": $LIBC_RSS,
          "pss": $LIBC_PSS
        },
        "others": {
          "rss": $OTHER_LIBS_RSS,
          "pss": $OTHER_LIBS_PSS
        },
        "total": {
          "rss": $SYSTEM_LIBS_RSS,
          "pss": $SYSTEM_LIBS_PSS
        },
        "units": "KB"
      }
    }
  },
  "percentages": {
    "rss": {
      "application_heap": $HEAP_PERCENT_RSS,
      "cryptographic_libraries": $CRYPTO_PERCENT_RSS,
      "networking_libraries": $CURL_PERCENT_RSS,
      "data_processing": $CJSON_PERCENT_RSS,
      "system_libraries": $SYSTEM_PERCENT_RSS
    },
    "pss": {
      "application_heap": $HEAP_PERCENT_PSS,
      "cryptographic_libraries": $CRYPTO_PERCENT_PSS,
      "networking_libraries": $CURL_PERCENT_PSS,
      "data_processing": $CJSON_PERCENT_PSS,
      "system_libraries": $SYSTEM_PERCENT_PSS
    }
  }
}
EOF

# Output CSV format for graphing
echo
echo "Component,RSS (KB),RSS %,PSS (KB),PSS %"
echo "Application Heap,$HEAP_RSS,$HEAP_PERCENT_RSS,$HEAP_PSS,$HEAP_PERCENT_PSS"
echo "Cryptographic Libraries,$CRYPTO_TOTAL_RSS,$CRYPTO_PERCENT_RSS,$CRYPTO_TOTAL_PSS,$CRYPTO_PERCENT_PSS"
echo "Networking (libcurl),$CURL_RSS,$CURL_PERCENT_RSS,$CURL_PSS,$CURL_PERCENT_PSS"
echo "JSON Processing,$CJSON_RSS,$CJSON_PERCENT_RSS,$CJSON_PSS,$CJSON_PERCENT_PSS"
echo "System Libraries,$SYSTEM_LIBS_RSS,$SYSTEM_PERCENT_RSS,$SYSTEM_LIBS_PSS,$SYSTEM_PERCENT_PSS"
echo "TOTAL,$TOTAL_RSS,100.0,$TOTAL_PSS,100.0"