#!/bin/bash

export PYTHONPATH=../../bindings/python         # assumes bindings have been built

# various bad and good cases
#./save_vector.py                               # missing arguments
#./save_vector.py -N20 /tmpx/myData.xco         # bad filename
#./save_vector.py -h                            # help
./save_vector.py -N20 myData.xco               # this should run
