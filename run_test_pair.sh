#!/bin/bash

# Ports
PORT1=8080
PORT2=8081

# Build the project
echo "Building the project..."
cmake -B build
if [ $? -ne 0 ]; then
    echo "CMake configuration failed."
    exit 1
fi

cmake --build build
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

# Check if executable exists
if [ ! -f "build/IOT" ]; then
    echo "Executable 'build/IOT' not found after build."
    exit 1
fi


#!/bin/bash

# Check if tmux is installed
if ! command -v tmux &> /dev/null
then
    echo "tmux could not be found. Please install it to use this script."
    exit 1
fi

# Ports
PORT1=8080
PORT2=8081

# Build the project
echo "Building the project..."
cmake -B build
if [ $? -ne 0 ]; then
    echo "CMake configuration failed."
    exit 1
fi

cmake --build build
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

# Check if executable exists
if [ ! -f "build/IOT" ]; then
    echo "Executable 'build/IOT' not found after build."
    exit 1
fi

# Create logs directory if it doesn't exist
mkdir -p logs

SESSION_NAME="iot_test_session"

# Kill any existing tmux session with the same name
tmux kill-session -t "$SESSION_NAME" 2>/dev/null

# Create a new detached tmux session
tmux new-session -d -s "$SESSION_NAME" -n "IOT Instances"

# Start the first instance in the first pane
tmux send-keys -t "$SESSION_NAME:0.0" "sh -c './build/IOT $PORT1 $PORT2 | tee logs/instance1.log'" C-m

# Split the window horizontally and start the second instance
tmux split-window -h -t "$SESSION_NAME:0.0"
tmux send-keys -t "$SESSION_NAME:0.1" "sh -c './build/IOT $PORT2 $PORT1 | tee logs/instance2.log'" C-m

# Attach to the tmux session
echo "Attaching to tmux session. Use 'Ctrl-b d' to detach."
echo "Use 'tmux kill-session -t $SESSION_NAME' to stop the instances."
sleep 1 # Give tmux a moment to start
tmux attach-session -t "$SESSION_NAME"
