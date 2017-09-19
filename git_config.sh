!/bin/bash

echo "Initializing Git!"
git init
echo "Authorizing Git!"
git remote add origin http://bitbucket.com/allstar_team/ifj-project.git # giving name origin
git pull origin master
git config --global credential.helper 'cache --timeout 7200' # you must log in every 2h
git branch --set-upstream-to=origin/master master # setting stream to origin/master branch
git pull

# setting mergetool
git config merge.tool meld
git config merge.conflictstyle meld

# getting files
git pull
