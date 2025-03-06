#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

#!/usr/bin/env bats

@test "check ls runs without errors" {
  run ./dsh <<EOF
ls
exit
EOF
  [ "$status" -eq 0 ]
}

@test "echo prints hello" {
  run ./dsh <<EOF
echo hello
exit
EOF
  echo "$output" | grep -q "hello"
  [ "$status" -eq 0 ]
}

@test "pipeline echo to grep" {
  run ./dsh <<EOF
echo hello | grep hello
exit
EOF
  echo "$output" | grep -q "hello"
  [ "$status" -eq 0 ]
}

@test "pipeline echo to tr" {
  run ./dsh <<EOF
echo hello | tr a-z A-Z
exit
EOF
  echo "$output" | grep -q "HELLO"
  [ "$status" -eq 0 ]
}

@test "echo with quotes prints hello world" {
  run ./dsh <<EOF
echo "hello world"
exit
EOF
  echo "$output" | grep -q "hello world"
  [ "$status" -eq 0 ]
}

@test "redirection output >" {
  run ./dsh <<EOF
echo "hello, class" > out.txt
cat out.txt
rm out.txt
exit
EOF
  echo "$output" | grep -q "hello, class"
  [ "$status" -eq 0 ]
}

@test "redirection output >> append" {
  run ./dsh <<EOF
echo "hello, class" > out.txt
echo "this is line 2" >> out.txt
cat out.txt
rm out.txt
exit
EOF
  echo "$output" | grep -q "hello, class"
  echo "$output" | grep -q "this is line 2"
  [ "$status" -eq 0 ]
}

@test "redirection input <" {
  run ./dsh <<EOF
echo "line from file" > infile.txt
cat < infile.txt
rm infile.txt
exit
EOF
  echo "$output" | grep -q "line from file"
  [ "$status" -eq 0 ]
}

@test "multiple pipeline commands" {
  run ./dsh <<EOF
echo hello | grep h | tr a-z A-Z
exit
EOF
  echo "$output" | grep -q "HELLO"
  [ "$status" -eq 0 ]
}

@test "nonexistent command returns error" {
  run ./dsh <<EOF
nonexistentcmd
exit
EOF
  [ "$status" -ne 0 ]
}

@test "built-in cd changes directory" {
  run ./dsh <<EOF
cd /
pwd
exit
EOF
  echo "$output" | grep -x "/"
  [ "$status" -eq 0 ]
}

@test "empty command returns warning" {
  run ./dsh <<EOF
  
exit
EOF
  echo "$output" | grep -q "warning: no commands provided"
  [ "$status" -eq 0 ]
}

@test "sequential commands without pipeline" {
  run ./dsh <<EOF
echo one
echo two
exit
EOF
  echo "$output" | grep -q "one"
  echo "$output" | grep -q "two"
  [ "$status" -eq 0 ]
}
