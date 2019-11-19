#!/bin/bash

# Get base path
SJBASE=$(dirname "$0")
SJBASE=$(cd "$SJBASE/.." ; pwd -P)
# Import SJSU-Dev2 common shell script functions
. $SJBASE/tools/common.sh

print_divider "Generating API Reference Documentation using Doxygen"

cd "$SJBASE/api/"
# Store everything from stderr (includes warnings and errors) and store it into
# the doxygen_stderr.txt file
doxygen sjsu-dev2-doxygen.conf 2> doxygen_stderr.txt

print_divider \
    "Verifying that Documentation is not Missing or Contains Mistakes"

grep --color --invert-match -f "$SJBASE/tools/doxygen_ignore_list.txt" \
    doxygen_stderr.txt

GREP_RETRUN_CODE=$?

# Grep will return 1 if it found nothing in the file and 0 if it found some
# matches. We want to match nothing, so a success is actually a return code of
# 1.
# So in this case, if we find a 1, return a zero to declare success!
if [[ "$GREP_RETRUN_CODE" -eq "1" ]]
then
	printf "$BOLD_GREEN"
  printf "Documentation Error Check Complete. Everything clear!"
  printf "$RESET\n"
  exit 0
fi

printf "$BOLD_RED"
printf "Documentation contains error. Please fix the issues listed above!"
printf "$RESET\n"

exit 1