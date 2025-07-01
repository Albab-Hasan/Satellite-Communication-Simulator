#!/bin/bash

# Script to build and run the Satellite Communication Simulator

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}===============================================================${NC}"
echo -e "${YELLOW}           SATELLITE COMMUNICATION SIMULATOR RUNNER            ${NC}"
echo -e "${YELLOW}===============================================================${NC}"
echo ""

# Check if make is installed
if ! command -v make &> /dev/null; then
    echo -e "${RED}Error: 'make' command not found. Please install make to build the project.${NC}"
    exit 1
fi

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}Error: 'g++' command not found. Please install g++ to build the project.${NC}"
    exit 1
fi

# Function to build the project
build_project() {
    echo -e "${YELLOW}Building project...${NC}"
    make clean
    make

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Build successful!${NC}"
        return 0
    else
        echo -e "${RED}Build failed. Please check the error messages above.${NC}"
        return 1
    fi
}

# Build the project
build_project
if [ $? -ne 0 ]; then
    echo -e "${YELLOW}Do you want to continue anyway? (y/n)${NC}"
    read continue_anyway

    if [[ $continue_anyway != "y" && $continue_anyway != "Y" ]]; then
        echo -e "${RED}Exiting...${NC}"
        exit 1
    fi
fi

# Run the simulator
echo -e "${YELLOW}Running Satellite Communication Simulator...${NC}"
echo ""
./sat-com

echo ""
echo -e "${GREEN}Thank you for using the Satellite Communication Simulator!${NC}"
exit 0
