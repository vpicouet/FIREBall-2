#!/bin/tcsh 
# 
# Launch an astrometry solution and parse the output so it can be
# easily ingested into the running C program. 
# 
# Usage:
#   run_astro <filename.fits> <ra> <dec> <radius>
#        <ra> : RA in degrees
#        <dec> : dec in degrees
#        <radius> : search radius in degrees
#

echo $1 $2 $3 $4

if ( $# != 4 ) then
    echo "Error: Insufficient arguments." >> /dev/stderr
    exit -1
endif 

set fitsname=$1
set ra=$2
set dec=$3
set radius=$4

set basename=`basename $fitsname .fits`


set fitspath=`dirname $fitsname`


set xyname="$basename.xy"
set solvedname="$fitspath/$basename.solved"
set solutionname="$fitspath/$basename.wcs"
set fireballsolve="$fitspath/$basename.fb"

if ( ` echo "$radius > 0" | bc `) then 
    set astroflags = "--ra $ra --dec $dec --radius $radius --cpulimit 30"
else 
    set astroflags = "--cpulimit 60"
endif

set scaleflags="--scale-units arcsecperpix --scale-low 0.8 --scale-high 1.0"
# set scaleflags=""

set pixflags="--crpix-x 640 --crpix-y 540"

set otherflags="--no-plots --no-verify --no-fits2fits --overwrite "

# Okay. If you want to use your own x-y extraction software, this is
# where it is invoked. Save the x-y list in the correct format for
# astrometry in $basename (i.e., fitsname.xy)


set commandline="solve-field  $scaleflags $pixflags $otherflags $astroflags $fitsname"

echo $commandline
set res=`solve-field $scaleflags $pixflags $astroflags $otherflags $fitsname >>& /dev/null`
set stat=$status
if ($status) then
    echo "Error: Astrometry failed."
    echo $stat
    exit 1
endif


# Exit with an error if not solved.
if ( ! -f $solvedname ) then
    echo "Failed to solve field." >> /dev/stderr
    exit 1
endif

set crval0=(`wcsinfo $solutionname | grep crval0`)
set crval1=(`wcsinfo $solutionname | grep crval1`)
set crval0=$crval0[2]
set crval1=$crval1[2]

set rot=(`wcsinfo $solutionname | grep "orientation "`)
set rot=$rot[2]

echo $crval0 $crval1 $rot >! $fireballsolve
echo "-----"
cat $fireballsolve


exit 0
