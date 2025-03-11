#!/bin/bash

# Stop the running container (if any)
docker stop splitr_dev

# Remove the existing container (if any)
docker rm splitr_dev

# Build the Docker image with the tag 'splitr'
docker build -t splitr .

# Run the Docker container with the necessary permissions and configurations
docker run -d --name splitr_dev \
  --privileged \
  --net=host \
  -v /var/run/dbus:/var/run/dbus \
  -v $(pwd):/app \
  splitr:latest

# Follow logs 
docker logs -f splitr_dev
