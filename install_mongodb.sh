#!/bin/sh

sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv EA312927
echo "deb http://repo.mongodb.org/apt/debian wheezy/mongodb-org/3.2 main" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.2.list
sudo apt-get update
sudo apt-get install -y mongodb-org



yum
wget https://fastdl.mongodb.org/linux/mongodb-linux-x86_64-rhel70-3.2.9.tgz

yum install -y mongodb-org-3.2.9 mongodb-org-server-3.2.9 mongodb-org-shell-3.2.9 mongodb-org-mongos-3.2.9 mongodb-org-tools-3.2.9
