#!/bin/bash

IMAGE_NAME="production"
IMAGE_TAG=$(date +%Y%m%d%H%M)
FULL_IMAGE_NAME="$IMAGE_NAME:$IMAGE_TAG"

echo "Building Docker image with tag $IMAGE_TAG..."
docker build -t $FULL_IMAGE_NAME .

echo "Cleaning up existing containers..."
docker rm -f $(docker ps -aq --filter ancestor=$IMAGE_NAME) 2> /dev/null

echo "Running Docker container..."
CONTAINER_ID=$(docker run -d --net=host --privileged $FULL_IMAGE_NAME)


# Check if the container is running before attaching
if [ $(docker inspect -f '{{.State.Running}}' $CONTAINER_ID) == "true" ]; then
    echo "Attaching to the Docker container's terminal..."
    docker attach $CONTAINER_ID
else
    echo "Container stopped unexpectedly. Check logs for details."
    docker logs $CONTAINER_ID
fi
