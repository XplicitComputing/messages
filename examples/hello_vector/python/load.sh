#!/bin/bash

export PYTHONPATH=../../bindings/python         # assumes bindings have been built

# various bad and good cases
#./load_vector.py                       # missing arguments
#./load_vector.py /tmpx/myData.xco      # bad filename
#./load_vector.py -h                    # help
./load_vector.py myData.xco            # this should run
