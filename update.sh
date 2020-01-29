#!/bin/bash
set -e

cd "$(dirname "$0")"

GITSTATUS=$(git pull)

if [ "Already up-to-date." != "$GITSTATUS" ]; then
   
   ./install.sh
   
else
   
   echo "No update available"
   
fi