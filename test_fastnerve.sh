#!/bin/bash
uvicorn server.api.main:app --port 8080 > server_output.log 2>&1 &
PID=$!
sleep 3
curl -s http://localhost:8080/probe > curl_output.log
kill $PID
wait $PID 2>/dev/null
cat server_output.log
echo "--- CURL OUTPUT ---"
cat curl_output.log
