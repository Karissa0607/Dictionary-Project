#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# write bash timeQ2.sh in terminal to run script.
# testing with 100 initial_rows and 10000 words see test_libDict.c for more details on testing.
echo "Time of Q2"
echo $(time $SCRIPT_DIR/q2/test_libDict)

exit 0
