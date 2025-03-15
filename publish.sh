#!/bin/bash

# Check if there are any uncommitted changes
if [[ -n $(git status --porcelain) ]]; then
    echo "Error: There are uncommitted changes in the repository."
    echo "Please commit or stash your changes before publishing."
    exit 1
fi

# Check if there are any unpushed commits
local_commit=$(git rev-parse HEAD)
remote_commit=$(git rev-parse @{upstream} 2>/dev/null || echo "no-upstream")

if [[ "$local_commit" != "$remote_commit" && "$remote_commit" != "no-upstream" ]]; then
    echo "Error: There are unpushed commits in the repository."
    echo "Please push your changes before publishing."
    exit 1
fi

echo "Repository is clean. Proceeding with publication..."

ssh root@jonathanzkoch.dev

cd cd Hosting/PortfolioWebsite/

git pull --force

sudo systemctl restart PortfolioWebsite.service

echo "Publication complete. Please verify the changes on the live server."

exit 0
