### choosePluginPrompt.bash ###
# For use by :
# 	installNuvuCameras.sh
# To guide the user through choosing a plugin

first_choice=0
choose_connector=4
connector=0

function driverAvailable # <GRABBER_ID>
{
	local grabber=$1

	local packagePath="${CURR_DIR}/${DRIVER_DIR}/$(printPackageDestinationPath $grabber)"
	test -d $packagePath
}

function publishAvailableDrivers
{
	local index=0

	echo -e "\t\t$index - All"
	CHOICE_ARRAY[$index]=ALL

	for grabber in $GRABBER_DEPENDENCIES
	do
		if 	driverAvailable $grabber
		then
			(( index += 1 ))
			echo -e "\t\t$index - $grabber"
			CHOICE_ARRAY[$index]=$grabber
		fi
	done

	#test -n $PLEORA_INSTALL && echo -e "\t\t1 - Pleora"
	#test -n $EDT_INSTALL && echo -e "\t\t2 - EDT"
	#test -n $MATROX_INSTALL && echo -e "\t\t3 - Matrox"

	(( index += 1 ))
	echo -e "\t\t${index} - I'm not sure.\n";
	CHOICE_ARRAY[$index]=CXN
	choose_connector=$index
}

VALID_CHOICE=no
until [ "$VALID_CHOICE" = 'yes' ]
do
	echo -e "Which frame-grabber, from the list below, has been supplied with your camera?"

	publishAvailableDrivers

	echo "Enter a number from the list :"
	read first_choice

	if [ $first_choice -ge 0 -a $first_choice -le ${choose_connector} ]
	then	
		VALID_CHOICE=yes
	else
		echo -e "Invalid choice; please try again.\n"
	fi
done

if [ $first_choice -eq ${choose_connector} ]
then	
	VALID_CHOICE=no
	gigE=1
	camL=2
	until [ "$VALID_CHOICE" = 'yes' ]
	do
		echo -e "How will your computer connect to your camera?"
		echo -e "\t\t1 - Ethernet cable"
		echo -e "\t\t2 - Camera Link cable\n";

		echo "Enter a number from the list :"
		read connector

		if [ $connector -ge $gigE -a $connector -le $camL ]
		then	
			VALID_CHOICE=yes
		else
			echo -e "Invalid choice; please try again.\n"
		fi
	done

	if [ $connector -eq $gigE ] 
	then
		if 	driverAvailable PLEORA
		then
			PLEORA_INSTALL=yes
		else
			echo -e "\n*** There is a problem with the contents of this installer (for internal dependency PLEORA): please contact Nuvu Cameras. ***"
			PLEORA_INSTALL=fail
		fi
	else
		for grabber in $GRABBER_DEPENDENCIES
		do
			if 	test "$grabber" != 'PLEORA' && driverAvailable $grabber
			then
				declare -g ${grabber}_INSTALL=yes 
			fi
		done
	fi
else
	if [ $first_choice -eq 0 ]
	then
		for grabber in $GRABBER_DEPENDENCIES
		do
			if 	driverAvailable $grabber
			then
				declare -g ${grabber}_INSTALL=yes 
			fi
		done
	else # grabber stored by index chosen
		declare -g ${CHOICE_ARRAY[$first_choice]}_INSTALL=yes 
	fi
fi


