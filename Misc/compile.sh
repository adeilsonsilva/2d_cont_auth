#!/bin/bash

g++ saveDatabaseFaces.cpp `pkg-config --cflags --libs opencv`
