### determineThirdPartyPath.bash ###
# For use by :
# 	installNuvuCameras.sh
# To determine a path with enough free space to satisfy REQUIRED_SPACE_BLOCKS
#  and, if necessary, prompt the user for intervention.

function existingRoot ()
{
	TEST_DIR=$1 
	until test -d $TEST_DIR
	do
		TEST_DIR=${TEST_DIR%/*}
	done
	echo -n $TEST_DIR
	return 0;
}
export -f existingRoot

function shortestNewPath ()
{
	NEW_PATH=$1
	NEW_PATH_ROOT=$(existingRoot $NEW_PATH)
	NEW_PATH=${NEW_PATH/${NEW_PATH_ROOT}\//}
	NEW_PATH=${NEW_PATH%%/*}
	if [ -z $NEW_PATH ]
	then # No new part of path; path is pre-existing
		return 1
	else
		echo -n "$NEW_PATH_ROOT/$NEW_PATH"
		return 0
	fi;
}
export -f shortestNewPath 

# Verify that the specified path has at least the specified number of blocks available
function verifyPathAvailableBlocks ()
{
	# To make df happy we truncate the specified path to the nearest existing directory 
	TEST_PATH=$(existingRoot $1)
	REQUIRED_BLOCKS=$2
	TEST_FS=$(df $TEST_PATH | grep -A1 dev) # May wrap if dev has a long name: recombine with echo ...
	AVAILABLE_BLOCKS=$(echo $TEST_FS | sed -r 's/\ +/\t/g' | cut -f 4)
	
	if [ $AVAILABLE_BLOCKS -lt $REQUIRED_BLOCKS ]
	then
		echo -e "\n*** Insufficient space available on ${TEST_PATH} ***"
		return 1
	else
		return 0
	fi;
}

# Verify that the specified path will allow rwx
function verifyPathAccess ()
{
	TEST_DIR=$1/.nuvu_install-test_third-party_temp
	TEST_ROOT=$(shortestNewPath $TEST_DIR)

	TEST_SCRIPT='#!/bin/bash\nexit 0'
	PERMISSIONS_FAIL=no

 	if ! mkdir -p $TEST_DIR
	then # Couldn't create directory
		PERMISSIONS_FAIL=yes
	elif ! echo -e "$TEST_SCRIPT"  >  $TEST_DIR/test.sh
	then # Couldn't write to file
		PERMISSIONS_FAIL=yes
	elif ! (chmod u+x $TEST_DIR/test.sh && $TEST_DIR/test.sh)
	then # Couldn't execute
		PERMISSIONS_FAIL=yes
	fi

	# Clean up
	rm -rf $TEST_ROOT

	if [ "$PERMISSIONS_FAIL" = 'yes' ]
	then
		echo -e "\n*** The path $1 does not have adequate permissions ***"
		return 1
	else
		return 0
	fi;
}

for THIRD_ROOT in $THIRD_LIST
do
	if [ "$THIRD_ROOT" = 'CHOICE' ]
	then
		echo "------------------------------------------------"
		df -h | head -n 1 | sed -r 's/\ +/\t/g' | cut -f 1,4,5,6 && df -h | grep "^/dev/[h,s]d" | sed -r 's/\ +/\t/g' | cut -f 1,4,5,6
		echo "------------------------------------------------"
		PATH_CONFIRM=no
		until [ "$PATH_CONFIRM" = 'yes' ]
		do
			echo "2G of available space are required."
			echo "Enter an absolute path :"
			read THIRD_PATH
			echo -n "- Confirm path $THIRD_PATH [Y/n] "
			read ANS 
			if [ "$ANS" = 'n' -o "$ANS" = 'N' -o "$ANS" = 'no' -o "$ANS" = 'No' -o "$ANS" = 'NO' ]
			then
				:
			else
				PATH_CONFIRM=yes
			fi
		done
	elif [ "$THIRD_ROOT" = "$THIRD_ALT" ]
	then 
		THIRD_PATH=${THIRD_ROOT}/nuvu-cameras/share
		echo -e "- We will attempt an installation to ${THIRD_PATH}: "
		echo -e "symbolic links from /opt/{provider} to ${THIRD_PATH}/{provider} will be created." 	
		echo -e "- If you do not want this, you will be presented with a list of the available partitions\nand the available size on each, then you will be given the option to specify a path."
		echo -n "- Would you like to install to ${THIRD_PATH}? [Y/n] "
		read ANS
		if [ "$ANS" = 'n' -o "$ANS" = 'N' -o "$ANS" = 'no' -o "$ANS" = 'No' -o "$ANS" = 'NO' ]
		then
			continue
	 	fi
	else
		THIRD_PATH=$THIRD_ROOT
	fi

	if ! (verifyPathAvailableBlocks $THIRD_PATH $REQUIRED_SPACE_BLOCKS && verifyPathAccess ${THIRD_PATH})
	then
		if [ "$PATH_CONFIRM" = 'yes' ]
		then # User chose this but still not good!
			echo -e "Installation of third-party packages failed!\n"
			echo -e "No chosen path had sufficient space!\n"
			exit 1
		else
			continue
		fi
	else
		break
	fi
done

