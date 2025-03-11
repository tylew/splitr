#!/bin/bash

container_name="splitr_dev"
force=false

# Parse optional flags
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --force) force=true ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done


if [ "$(docker ps -a -q -f name=$container_name)" ] && [ "$force" = false ]; then
    echo "Restarting Docker container: $container_name"
    docker stop splitr_dev
    docker start splitr_dev
else
    echo "Creating new Docker container: $container_name"
fi

# Follow logs 
docker logs -f splitr_dev
