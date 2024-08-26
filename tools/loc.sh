#!/bin/bash

project_dir=$(dirname $(dirname $(realpath $0)))

cloc --exclude-dir 3rdparty,.vscode --exclude-ext gpx,fit $project_dir

