#!/bin/bash
rsync -av --exclude='__pycache__' --files-from=files.txt . /Users/rayanenasri/Documents/Internships/aGrUMLab/.venv/lib/python3.13/site-packages/pyagrum
