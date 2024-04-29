### depList.bash ###
# For use by :
# 	Makefile.linux 
# 	installNuvuCameras.sh
# List of variables identifying os-versions / packages / installers for dependencies
# TODO: perl would be great for all this ...
#       Also, bash4 has associative arrays (via 'declare -A <var>') ...


#############################
# OS-agnostic definitions 

# Names of variables used to identify paths to packages
# NOTE: GRABBER_DEPENDENCIES may be better named PLUGIN_DEPENDENCIES
# TODO; CFITSIO may be unnecessary as of 4.2 due to use of nc_image_io ...
GRABBER_DEPENDENCIES='PLEORA EDT MATROX'
EXTRA_DEPENDENCIES='CFITSIO'

DEPENDENCIES="$GRABBER_DEPENDENCIES $EXTRA_DEPENDENCIES"

# Extra dependency directories :
# 	( SOURCE_DIR DESTINATION_DIR )
CFITSIO='cfitsio cfitsio'
# cfitsio is a generic package so we provide a default value that may be overriden
# It must be of type .tar.gz 
#  and the package must obey the configure; make; make install paradigm.
CFITSIO_DEFAULT=cfitsio3340.tar.gz


# Frame-grabber driver directories :
# 	( SOURCE_DIR DESTINATION_DIR NC_INSTALL_SCRIPT PLUGIN_DIR )
EDT='EDT EDTpdv 		nc-install-edt.bash    nc_grab_edt    '
PLEORA='Pleora pleora 		nc-install-pleora.bash nc_grab_pleora '
MATROX='MIL-Lite matrox_imaging nc-install-matrox.bash nc_grab_mil '


#############################
# OS-relevant definitions 

FLAVOURS=( RHB UBT )
# For each of {FLAVOURS} define :
# 	{FLAVOURS}_VERSIONS
#  and :
# 	{DEPENDENCIES}_{FLAVOURS}_PKGS
#  where :
# 	DEPENDENCIES=( $GRABBER_DEPENDENCIES $EXTRA_DEPENDENCIES )

# TODO: ALso define a plugin tag for each plugin-related installer

# Redhat-based, i.e. Enterprise Linux versions
RHB_6='EL6'
RHB_7='EL7'
RHB_VERSIONS=( "$RHB_6" "$RHB_7" )

#RHB_6_EDT_PKG=EDTpdv-5.5.2-1.noarch.rpm # NOTE: nc-install-edt.bash previously forced use of EDTpdv_lnx_5.5.2.1.run! 
RHB_6_EDT_PKG=EDTpdv_lnx_5.5.2.1.run
RHB_7_EDT_PKG=EDTpdv-5.5.8-2.noarch.rpm
EDT_RHB_PKGS=( "$RHB_6_EDT_PKG" "$RHB_7_EDT_PKG" )

RHB_6_PLEORA_PKG=eBUS_SDK_4.0.6.3228_RHEL-6-x86_64.run # NOTE: No newer download available for 4.0
#RHB_7_PLEORA_PKG=eBUS_SDK_CentOS-RHEL-7-x86_64-5.0.0-4100.rpm
#RHB_7_PLEORA_PKG=eBUS_SDK_RHEL-CentOS-7-x86_64-5.1.10-4642.rpm # NOTE: optional; build untested
RHB_7_PLEORA_PKG=eBUS_SDK_RHEL-CentOS-7-x86_64-6.0.2-4879.rpm # NOTE: optional; build untested
PLEORA_RHB_PKGS=( "$RHB_6_PLEORA_PKG" "$RHB_7_PLEORA_PKG" )

RHB_6_MATROX_PKG=mil-lite-10.00_2635-installer64.run
MATROX_RHB_PKGS=( "$RHB_6_MATROX_PKG" "" )


RHB_6_CFITSIO_PKG=cfitsio3340.tar.gz
RHB_7_CFITSIO_PKG=cfitsio3340.tar.gz
CFITSIO_RHB_PKGS=( "$RHB_6_CFITSIO_PKG" "$RHB_7_CFITSIO_PKG" )



# Ubuntu versions
UBT_12='U12' # 12.04 LTS
UBT_16='U16' # 16.04 LTS
UBT_18='U18' # 18.04 LTS
UBT_VERSIONS=( "$UBT_12" "$UBT_16" "$UBT_18" )

UBT_12_EDT_PKG=EDTpdv_lnx_5.5.8.2.run
UBT_16_EDT_PKG=EDTpdv_lnx_5.5.8.2.run
#UBT_18_EDT_PKG=EDTpdv_lnx_5.5.8.2.run
UBT_18_EDT_PKG=EDTpdv_lnx_5.5.8.9.run # NOTE: automatically handle system kernel updates from 5.5.8.8
EDT_UBT_PKGS=( "$UBT_12_EDT_PKG" "$UBT_16_EDT_PKG" "$UBT_18_EDT_PKG" )

UBT_12_PLEORA_PKG=eBUS_SDK_4.0.10.3634_Ubuntu-12.04-x86_64.run # NOTE: executable *not* actual package
UBT_16_PLEORA_PKG=eBUS_SDK_Ubuntu-x86_64-5.0.0-4100.deb
#UBT_16_PLEORA_PKG=eBUS_SDK_Ubuntu-x86_64-5.1.10-4642.deb # NOTE: optional; build untested
#UBT_16_PLEORA_PKG=eBUS_SDK_Ubuntu-x86_64-6.0.2-4879.deb # NOTE: optional; build untested
UBT_18_PLEORA_PKG=eBUS_SDK_Ubuntu-x86_64-6.0.2-4879.deb # NOTE: NOT optional;build untested
PLEORA_UBT_PKGS=( "$UBT_12_PLEORA_PKG" "$UBT_16_PLEORA_PKG" "$UBT_18_PLEORA_PKG" ) # Build with Ubuntu 18 untested ...

UBT_12_MATROX_PKG=mil-lite-10.00_2635-installer64.run # NOTE: MIL-Lite 10 installer requires kernel 3.8.0 ('Raring' Hardware Enablement Stack; kernel matches Ubuntu 13.04; official support for this stack expires 08/2014 to be replaced by kernel 3.13 matching Ubuntu 14.04)
MATROX_UBT_PKGS=( "$UBT_12_MATROX_PKG" "" "" )


UBT_12_CFITSIO_PKG=cfitsio3340.tar.gz
UBT_16_CFITSIO_PKG=cfitsio3340.tar.gz
UBT_18_CFITSIO_PKG=cfitsio3340.tar.gz
CFITSIO_UBT_PKGS=( "$UBT_12_CFITSIO_PKG" "$UBT_16_CFITSIO_PKG" "$UBT_18_CFITSIO_PKG" )

# TODO: Check that size of each *_PKG array matches that of the appropriate *_VERSIONS array

#############################
# OS-agnostic tools (for use via Makefile.macosx)

# 
function printSupportedGrabberInstallScripts
{
	for grabberVarName in $GRABBER_DEPENDENCIES 
	do
		local grabberPathArrayContentsString="${grabberVarName}[@]"
		local grabberPathArray=( ${!grabberPathArrayContentsString} )

		echo -n -e "${grabberVarName}/${grabberPathArray[2]} "
	done
}

function printSupportedGrabberPlugins
{
	for grabberVarName in $GRABBER_DEPENDENCIES 
	do
		local grabberPathArrayContentsString="${grabberVarName}[@]"
		local grabberPathArray=( ${!grabberPathArrayContentsString} )

		echo -n -e "${grabberVarName}/${grabberPathArray[3]} \t"
	done
}

function printSupportedGrabberPluginsDestinations
{
	for grabberVarName in $GRABBER_DEPENDENCIES 
	do
		local grabberPathArrayContentsString="${grabberVarName}[@]"
		local grabberPathArray=( ${!grabberPathArrayContentsString} )

		echo -n -e "${grabberPathArray[1]}/${grabberPathArray[3]} \t"
	done
}

function printSupportedGrabberDrivers
{
	for name in $GRABBER_DEPENDENCIES 
	do
		echo -n -e "${name} \t"
	done
}

function printSupportedExtraDependencies
{
	for name in $EXTRA_DEPENDENCIES 
	do
		echo -n -e "${name} \t"
	done
}
