#!/bin/bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/sugma.ssh
git add .
echo "Enter your commit message: "
read commit_message
git commit -m $commit_message
clear
echo "Committing updates to github"
git push -u origin main --force
# qwen --resume 5ffa600c-6d7a-44b4-8234-f4e01b374a9a