#!/bin/bash

# System status check script written by Knight Yang

# Define the log file path
LOGFILE="$HOME/system_status_check.log"

# Function to log messages with a timestamp
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" >> "$LOGFILE"
}

# Check if the log file directory is writable
if [ ! -w "$(dirname "$LOGFILE")" ]; then
    log "Error: Unable to write to log file."
    exit 1
fi

# Check for the existence of required commands
required_commands=("hostname" "awk" "free" "df" "uptime" "lscpu" "uname" "who" "cat")
for cmd in "${required_commands[@]}"; do
    if ! command -v "$cmd" &> /dev/null; then
        log "Error: Command '$cmd' is required but not found."
        exit 1
    fi
done

# Function to display system information
display_system_info() {
    # Get IP address and hostname
    IP_ADDR=$(hostname -I | cut -d' ' -f1)
    HOSTNAME=$(hostname)
    
    # Get CPU model name and remove leading spaces and tabs
    CPU_MODEL=$(lscpu | awk -F': ' '/^Model name:/ {sub(/^[ \t]+/, ""); print $2}')

    # Trim any remaining leading spaces, just in case
    CPU_MODEL=$(echo "$CPU_MODEL" | sed 's/^[ \t]*//')


    # Log and display basic system information
    log "Starting basic system information check."
    echo
    echo -e "\tBasic System Information:"
    echo -e "\t------------------------------------------------"
    echo -e "\tCurrent Time : $(date)"
    echo -e "\tVersion      : $(cat /etc/os-release | grep -w "PRETTY_NAME" | cut -d= -f2 | tr -d '"')"
    echo -e "\tKernel       : $(uname -r)"
    echo -e "\tUptime       : $(uptime -p)"
    echo -e "\tIP addr      : $IP_ADDR"
    echo -e "\tHostname     : $HOSTNAME"
    echo -e "\tCPU          : $CPU_MODEL"
    echo -e "\tMemory       : $(free -h | awk '/^Mem:/ { print $3 "/" $2 }')"
    echo -e "\tSWAP         : $(free -h | awk '/^Swap:/ { print $3 "/" $2 }')"
    echo -e "\tUsers Logged : $(who | wc -l) users"
    echo
    log "Completed basic system information check."
}

# Function to display CPU load information
display_cpu_load() {
    log "Starting CPU load information check."
    echo -e "\tCPU Load Information:"
    echo -e "\t------------------------------------------------"
    echo -e "\tCPU load in 1 min is   : $(awk '{printf "%15s", $1}' /proc/loadavg)"
    echo -e "\tCPU load in 5 mins is  : $(awk '{printf "%15s", $2}' /proc/loadavg)"
    echo -e "\tCPU load in 15 mins is : $(awk '{printf "%15s", $3}' /proc/loadavg)"
    echo
    log "Completed CPU load information check."
}

# Function to display memory information
display_memory_info() {
    log "Starting memory information check."
    echo -e "\tMemory Usage Information:"
    echo -e "\t------------------------------------------------"
    echo -e "\tTotal Memory  : $(free -h | awk '/Mem/{print $2}')"
    echo -e "\tFree Memory   : $(free -h | awk '/Mem/{print $4}')"
    echo -e "\tCached Memory : $(free -h | awk '/Mem/{print $6}')"
    echo
    log "Completed memory information check."
}

# Function to rank disk usage
rank_disk_usage() {
    log "Starting disk usage ranking check."
    echo -e "\tDisk Usage Ranking:"
    echo -e "\t------------------------------------------------"
    df -h -x tmpfs -x devtmpfs | sort -nr -k 5 | awk '/dev/{printf "\t%-39s %5s\n", $1, $5}'
    echo
    log "Completed disk usage ranking check."
}

# Main execution logic
log "Script execution started."
display_system_info
display_cpu_load
display_memory_info
rank_disk_usage
log "Script execution completed."
