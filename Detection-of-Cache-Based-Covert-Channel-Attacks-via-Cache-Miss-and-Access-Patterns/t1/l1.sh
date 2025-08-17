#!/usr/bin/env bash
# baseline.sh — run sender under various CPU loads to collect access counts

OUTPUT="cache_baseline.csv"
echo "load_level,accesses" > "$OUTPUT"

# number of CPU‐burn workers
LOADS=(1 4 8 12)

for L in "${LOADS[@]}"; do
  echo "Running for load level $L"

  STRESS_PIDS=()

  if (( L > 0 )); then
    for (( i=0; i<L; i++ )); do
      # Each instance creates multiple stressors
      stress-ng \
        --cpu 2 \
        --vm 2 --vm-bytes 512M \
        --stream 2 \
        --cache 2 --cache-level 3 \
        --timeout 30s > /dev/null 2>&1 &
      STRESS_PIDS+=($!)
    done
    # sleep 3  # give time for stressors to kick in
  fi

  for RUN in {1..5}; do
    ACCESS=$(taskset -c 0 ./sender)
    echo "$L: $ACCESS"
    echo "$L: $ACCESS" >> "$OUTPUT"
    sleep $((RANDOM % 2 + 1))
  done

  if (( L > 0 )); then
    for pid in "${STRESS_PIDS[@]}"; do
      kill "$pid" 2>/dev/null
      wait "$pid" 2>/dev/null
    done
  fi
done

echo "Baseline data written to $OUTPUT"
