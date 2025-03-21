#!/usr/bin/env bats

############################ DO NOT EDIT THIS FILE #####################################
# File: student_tests.sh
#
# This file contains tests for the Remote Drexel Shell assignment.
# DO NOT EDIT THIS FILE.
#
# All tests in this file must pass - it is used as part of grading!
########################################################################################

@test "Local mode displays local mode message" {
  run bash -c 'echo "exit" | ./dsh'
  [ "$status" -eq 0 ]
  [[ "$output" =~ "local mode" ]]
}

@test "Help message displays usage information" {
  run ./dsh -h
  [ "$status" -eq 0 ]
  [[ "$output" =~ "Usage:" ]]
}

@test "Error when using -i option in local mode" {
  run ./dsh -i 127.0.0.1 2>&1
  [ "$status" -ne 0 ]
  [[ "$output" =~ "Error: -i can only be used with -c or -s" ]]
}

@test "Error when using -p option in local mode" {
  run ./dsh -p 5678 2>&1
  [ "$status" -ne 0 ]
  [[ "$output" =~ "Error: -p can only be used with -c or -s" ]]
}

@test "Error when using both -c and -s options" {
  run ./dsh -c -s 2>&1
  [ "$status" -ne 0 ]
  [[ "$output" =~ "Error: Cannot use both -c and -s" ]]
}

@test "Error when using -x (threaded) in client mode" {
  run ./dsh -c -x 2>&1
  [ "$status" -ne 0 ]
  [[ "$output" =~ "Error: -x can only be used with -s" ]]
}

@test "Socket server mode (default parameters) stops on 'stop-server'" {
  ./dsh -s > server_output.txt 2>&1 &
  server_pid=$!
  sleep 0.5
  run bash -c 'echo "stop-server" | ./dsh -c'
  [ "$status" -eq 0 ]
  wait "$server_pid"
  server_output=$(cat server_output.txt)
  rm -f server_output.txt
  [[ "$server_output" =~ "socket server mode:" ]]
  [[ "$server_output" =~ "addr:0.0.0.0:1234" ]]
  [[ "$server_output" =~ "Single-Threaded Mode" ]]
}

@test "Socket server mode with overridden IP and port stops on 'stop-server'" {
  ./dsh -s -i 0.0.0.0 -p 7890 > server_output.txt 2>&1 &
  server_pid=$!
  sleep 0.5
  run bash -c 'echo "stop-server" | ./dsh -c -i 127.0.0.1 -p 7890'
  [ "$status" -eq 0 ]
  wait "$server_pid"
  server_output=$(cat server_output.txt)
  rm -f server_output.txt
  [[ "$server_output" =~ "socket server mode:" ]]
  [[ "$server_output" =~ "addr:0.0.0.0:7890" ]]
}

@test "Socket client mode (default parameters) connects and exits" {
  ./dsh -s > /dev/null 2>&1 &
  server_pid=$!
  sleep 0.5
  run bash -c 'echo "exit" | ./dsh -c'
  kill "$server_pid" 2>/dev/null
  [ "$status" -eq 0 ]
  [[ "$output" =~ "socket client mode:" ]]
}

@test "Socket client mode with overridden IP and port connects and exits" {
  ./dsh -s -i 0.0.0.0 -p 7890 > /dev/null 2>&1 &
  server_pid=$!
  sleep 0.5
  run bash -c 'echo "exit" | ./dsh -c -i 127.0.0.1 -p 7890'
  kill "$server_pid" 2>/dev/null
  [ "$status" -eq 0 ]
  [[ "$output" =~ "socket client mode:" ]]
}

