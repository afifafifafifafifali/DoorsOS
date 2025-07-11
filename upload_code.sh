#!/bin/bash
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/sugma.ssh
git add .
echo "Enter your commit message: "
read commit_message
git commit -m $commit_message
clear
echo "Committing updates to github"
git push origin main

