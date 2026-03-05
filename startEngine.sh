#!/bin/bash

PORT=$1

g++ source_code/engine/engine.cpp -o engine

./engine $PORT
