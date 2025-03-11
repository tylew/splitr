#!/bin/bash

# Define the project directory as location of this script
PROJECT_DIR=$(dirname "$(readlink -f "$0")")

# Navigate to the project directory
cd $PROJECT_DIR

# Check if the project directory exists
if [ ! -d "$PROJECT_DIR" ]; then
    echo "Error: Project directory does not exist."
    exit 1
fi

# Pull the latest changes from the repository
echo "Pulling the latest changes from the Git repository..."
git pull origin master

# Check if git pull was successful
if [ $? -ne 0 ]; then
    echo "Failed to pull latest changes. Please check your Git configuration."
    exit 1
fi

echo "Git pull completed successfully."
