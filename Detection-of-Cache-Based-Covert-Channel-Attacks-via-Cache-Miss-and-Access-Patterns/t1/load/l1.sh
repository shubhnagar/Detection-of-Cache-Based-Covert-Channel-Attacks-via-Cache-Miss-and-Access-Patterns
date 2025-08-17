#!/usr/bin/env bash
# baseline.sh — run sender under various CPU loads to collect access counts

OUTPUT="cache_baseline.csv"
echo "load_level,accesses" > "$OUTPUT"

# number of CPU‐burn workers
LOADS=(0 5 11)

for L in "${LOADS[@]}"; do
  if (( L > 0 )); then
    stress-ng --cpu "$L" --timeout 30s > /dev/null 2>&1 &
    STRESS_PID=$!
    sleep 1
  fi

  for RUN in {1..20}; do
    ACCESS=$(./../sender)             # prints a single number
    echo "$L,$ACCESS" >> "$OUTPUT"
  done

  if (( L > 0 )); then
    kill $STRESS_PID
    wait $STRESS_PID 2>/dev/null
  fi
done

echo "Baseline data written to $OUTPUT"
