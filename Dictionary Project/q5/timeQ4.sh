#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# write bash timeQ4.sh in terminal to run script.
# testing with 100 initial_rows and 10000 words check test_libDict.c for more details on testing.
echo "Time of Q4"
echo $(time $SCRIPT_DIR/q4/test_libDict)

exit 0
