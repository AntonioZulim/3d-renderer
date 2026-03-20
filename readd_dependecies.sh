#!/bin/sh

while read url path commit;
do
	git submodule add $url $path || echo "Vec je postojao $path"
	cd $path
	git fetch --all --tags
	git checkout $commit
	cd ../../..
	git add $path
done < dependecies.txt