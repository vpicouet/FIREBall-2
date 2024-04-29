
declare -A PKG_LIST
function appendPackageList # <VERSIONS> <PACKAGE_ID> <PACKAGES>
{
	local versionsArrayName=$1
	local versionsArrayContentsString="${versionsArrayName}[@]"

	local grabberVarName=$2
	
	local packagesArrayName=$3
	local packagesArrayContentsString="${packagesArrayName}[@]"
	local packagesArray=( "${!packagesArrayContentsString}" )

	#echo "Extending package list for $grabberVarName ..."
	local index=0
	for version in "${!versionsArrayContentsString}"
	do
		local key="${grabberVarName}_${version}"
		local pkg_name="${packagesArray[$index]}"
		(( index += 1 ))

		#echo -e "PKG_LIST[ $key ] \t=  $pkg_name"
		PKG_LIST["$key"]=$pkg_name
	done
}
#TODO: Automate the following via loop on DEPENDENCIES;
#       all by indirection ... so also loop on ( RHB UBT )
appendPackageList 'RHB_VERSIONS' 'EDT' 'EDT_RHB_PKGS'
appendPackageList 'RHB_VERSIONS' 'PLEORA' 'PLEORA_RHB_PKGS'
appendPackageList 'RHB_VERSIONS' 'MATROX' 'MATROX_RHB_PKGS'

appendPackageList 'RHB_VERSIONS' 'CFITSIO' 'CFITSIO_RHB_PKGS' # NOTE: Shouldn't be strictly necessary

appendPackageList 'UBT_VERSIONS' 'EDT' 'EDT_UBT_PKGS'
appendPackageList 'UBT_VERSIONS' 'PLEORA' 'PLEORA_UBT_PKGS'
appendPackageList 'UBT_VERSIONS' 'MATROX' 'MATROX_UBT_PKGS'

appendPackageList 'UBT_VERSIONS' 'CFITSIO' 'CFITSIO_UBT_PKGS' # NOTE: Shouldn't be strictly necessary

#declare -A EDT_RHB_PKGS
#EDT_RHB_PKGS=( [$RHB_6]="${RHB_6_EDT_PKG}" [$RHB_7]="${RHB_7_EDT_PKG}" )

#declare -A RHB_PKG_LISTS
#RHB_PKG_LISTS=( [$EDT]='EDT_RHB_PKGS' [$PLEORA]='PLEORA_RHB_PKGS' [$MATROX]='MATROX_RHB_PKGS' )



# Print the package name 
#  providing a default value from <PACKAGE_ID>_DEFAULT  
#  if no specific package is defined.
function printPackageName # <PACKAGE_ID> <OS> 
{
        local depVarName=$1
	local osVersion=$2

	local pkgVarName="${depVarName}_DEFAULT"
	local pkgName="${PKG_LIST["${depVarName}_${osVersion}"]}"

	if 	test -z $pkgName
	then # No package selected; return default
		echo "${!pkgVarName}"
	else
		echo "$pkgName"
	fi
}

#echo "Package for EDT with $RHB_7: $(printPackageName 'EDT' "$RHB_7")"

function printPackageSourcePath # <PACKAGE_ID> <OS>  
{
        local depVarName=$1
	local osVersion=$2
	#echo "${!depVarName}/$(printPackageName $depVarName $osVersion)"

	local depPathArrayContentsString="${depVarName}[@]"
	local depPathArray=( ${!depPathArrayContentsString} )

	echo "${depPathArray[0]}/$(printPackageName $depVarName $osVersion)"
}
#echo "Package for EDT with $RHB_7 found in: $(printPackageSourcePath 'EDT' "$RHB_7")"

# TODO : No longer depends on <OS>; clarify use in situ.
function printPackageDestinationPath # <PACKAGE_ID>
{
        local depVarName=$1
	#local osVersion=$2
	#echo "${!depVarName}/$(printPackageName $depVarName $osVersion)"

	local depPathArrayContentsString="${depVarName}[@]"
	local depPathArray=( ${!depPathArrayContentsString} )

	echo "${depPathArray[1]}"
}
#echo "Package for CFITSIO with $RHB_7 found in: $(printPackageSourcePath 'CFITSIO' "$RHB_7") and sent to $(printPackageDestinationPath 'CFITSIO' "$RHB_7") "



function printSupportedPackages
{
	for key in ${!PKG_LIST[@]} # NOTE: keys recovered in ~random order
	do
		echo -e "$key \tsupported via \t${PKG_LIST[$key]}"
	done
}

#printSupportedPackages

# List of all required packages
function printRequiredPackageSources # <OS> [PREPEND_PATH] 
{
	local osVersion=$1
	local prePath=$2
	
	local dependencies=( $GRABBER_DEPENDENCIES $EXTRA_DEPENDENCIES )

	for varName in "${dependencies[@]}"
	do
		if test -n "$prePath"
		then
			echo -n "$prePath/$(printPackageSourcePath $varName $osVersion) "
		else
			echo -n "$(printPackageSourcePath $varName $osVersion) "
		fi
	done
}
#echo "Packages for EL7: $(printRequiredPackageSources 'EL7' )"
#echo "Packages for UB12: $(printRequiredPackageSources 'U12' 'MNT_LOGLX')"

