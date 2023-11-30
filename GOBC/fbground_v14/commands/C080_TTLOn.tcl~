set ns "C080"
namespace eval $ns {
variable commandID 80
variable shutterAName "Not Used"
variable shutterBName "UFO Shutter"
variable shutterCName "Zinc Lamp Shutter"
variable shutterDName "D2 Lamp Shutter"
variable lampAName "Zinc"
variable lampBName "D2"


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P080Tab

# -----------------------------
# frame for shutter A 
# -----------------------------
ttk::labelframe $tab.tshutA \
    -borderwidth 2 \
    -relief ridge \
    -text $shutterAName

ttk::labelframe $tab.tshutB \
    -borderwidth 2 \
    -relief ridge \
    -text $shutterBName

ttk::labelframe $tab.tshutC \
    -borderwidth 2 \
    -relief ridge \
    -text $shutterCName

ttk::labelframe $tab.tshutD \
    -borderwidth 2 \
    -relief ridge \
    -text $shutterDName

ttk::labelframe $tab.tlampA \
    -borderwidth 2 \
    -relief ridge \
    -text $lampAName

ttk::labelframe $tab.tlampB \
    -borderwidth 2 \
    -relief ridge \
    -text $lampBName

button $tab.tshutA.bshutAOn\
    -text "On"\
    -font {Helvetica 12 bold}\
    -command "${ns}::onfunction 1"

button $tab.tshutA.bshutAOff\
    -text "Off"\
    -font {Helvetica 12 bold}\
    -command "${ns}::offfunction 1"

ttk::label $tab.tshutA.tshutAtxt\
    -text "Shut A status: "\
    -justify right

ttk::label $tab.tshutA.tshutAvar\
    -textvariable var::shutAstatus\
    -anchor center\
    -relief sunken\
    -width 10

button $tab.tshutB.bshutBOn\
    -text "Close"\
    -font {Helvetica 12 bold}\
    -command "${ns}::onfunction 2"

button $tab.tshutB.bshutBOff\
    -text "Open"\
    -font {Helvetica 12 bold}\
    -command "${ns}::offfunction 2"

ttk::label $tab.tshutB.tshutBtxt\
    -text "Shut B status: "\
    -justify right

ttk::label $tab.tshutB.tshutBvar\
    -textvariable var::shutBstatus\
    -anchor center\
    -relief sunken\
    -width 10

button $tab.tshutC.bshutCOn\
    -text "Open"\
    -font {Helvetica 12 bold}\
    -command "${ns}::onfunction 4"

button $tab.tshutC.bshutCOff\
    -text "Close"\
    -font {Helvetica 12 bold}\
    -command "${ns}::offfunction 4"

ttk::label $tab.tshutC.tshutCtxt\
    -text "Shut C status: "\
    -justify right

ttk::label $tab.tshutC.tshutCvar\
    -textvariable var::shutCstatus\
    -anchor center\
    -relief sunken\
    -width 10

button $tab.tshutD.bshutDOn\
    -text "Open"\
    -font {Helvetica 12 bold}\
    -command "${ns}::onfunction 8"

button $tab.tshutD.bshutDOff\
    -text "Close"\
    -font {Helvetica 12 bold}\
    -command "${ns}::offfunction 8"

ttk::label $tab.tshutD.tshutDtxt\
    -text "Shut D status: "\
    -justify right

ttk::label $tab.tshutD.tshutDvar\
    -textvariable var::shutDstatus\
    -anchor center\
    -relief sunken\
    -width 10

button $tab.tlampA.blampAOn\
    -text "On"\
    -font {Helvetica 12 bold}\
    -command "${ns}::onfunction 16"

button $tab.tlampA.blampAOff\
    -text "Off"\
    -font {Helvetica 12 bold}\
    -command "${ns}::offfunction 16"


ttk::label $tab.tlampA.tlampAtxt\
    -text "Lamp A status: "\
    -justify right

ttk::label $tab.tlampA.tlampAvar\
    -textvariable var::lampAstatus\
    -anchor center\
    -relief sunken\
    -width 10

button $tab.tlampB.blampBOn\
    -text "On"\
    -font {Helvetica 12 bold}\
    -command "${ns}::onfunction 64"

button $tab.tlampB.blampBOff\
    -text "Off"\
    -font {Helvetica 12 bold}\
    -command "${ns}::offfunction 64"

ttk::label $tab.tlampB.tlampBtxt\
    -text "Lamp B status: "\
    -justify right

ttk::label $tab.tlampB.tlampBvar\
    -textvariable var::lampBstatus\
    -anchor center\
    -relief sunken\
    -width 10



ttk::labelframe $tab.tsensor \
    -borderwidth 2 \
    -relief ridge \
    -text "Sensors"

ttk::label $tab.tsensor.tsensorAtxt\
    -text "Calib. Box Temperature (C):"\
    -justify right

ttk::label $tab.tsensor.tsensorAvar\
    -textvariable var::ai_cal_temp\
    -anchor center\
    -relief sunken\
    -width 10

ttk::label $tab.tsensor.tsensorBtxt\
    -text "Calib. Box Pressure (mbar): "\
    -justify right

ttk::label $tab.tsensor.tsensorBvar\
    -textvariable var::ai_cal_pressure\
    -anchor center\
    -relief sunken\
    -width 10

ttk::label $tab.tsensor.tsensorCtxt\
    -text "Sensor C status: "\
    -justify right

ttk::label $tab.tsensor.tsensorCvar\
    -textvariable var::sensorCstatus\
    -anchor center\
    -relief sunken\
    -width 10

grid $tab.tsensor.tsensorAtxt\
    -in $tab.tsensor\
    -row 0\
    -column 0

grid $tab.tsensor.tsensorAvar\
    -in $tab.tsensor\
    -row 0\
    -column 1

grid $tab.tsensor.tsensorBtxt\
    -in $tab.tsensor\
    -row 1\
    -column 0

grid $tab.tsensor.tsensorBvar\
    -in $tab.tsensor\
    -row 1\
    -column 1

grid $tab.tsensor.tsensorCtxt\
    -in $tab.tsensor\
    -row 2\
    -column 0

grid $tab.tsensor.tsensorCvar\
    -in $tab.tsensor\
    -row 2\
    -column 1



grid $tab.tshutA.bshutAOn\
    -in $tab.tshutA\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5

grid $tab.tshutA.bshutAOff\
    -in $tab.tshutA\
    -row 1\
    -column 1\
    -padx 5\
    -pady 5


grid $tab.tshutA.tshutAtxt\
    -in $tab.tshutA\
    -row 1\
    -column 2\
    -padx 5\
    -pady 5

grid $tab.tshutA.tshutAvar\
    -in $tab.tshutA\
    -row 1\
    -column 3\
    -padx 5\
    -pady 5

grid $tab.tshutB.bshutBOn\
    -in $tab.tshutB\
    -row 1\
    -column 1\
    -padx 5\
    -pady 5

grid $tab.tshutB.bshutBOff\
    -in $tab.tshutB\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5

grid $tab.tshutB.tshutBtxt\
    -in $tab.tshutB\
    -row 1\
    -column 2\
    -padx 5\
    -pady 5

grid $tab.tshutB.tshutBvar\
    -in $tab.tshutB\
    -row 1\
    -column 3\
    -padx 5\
    -pady 5

grid $tab.tshutC.bshutCOn\
    -in $tab.tshutC\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5

grid $tab.tshutC.bshutCOff\
    -in $tab.tshutC\
    -row 1\
    -column 1\
    -padx 5\
    -pady 5

grid $tab.tshutC.tshutCtxt\
    -in $tab.tshutC\
    -row 1\
    -column 2\
    -padx 5\
    -pady 5

grid $tab.tshutC.tshutCvar\
    -in $tab.tshutC\
    -row 1\
    -column 3\
    -padx 5\
    -pady 5

grid $tab.tshutD.bshutDOn\
    -in $tab.tshutD\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5

grid $tab.tshutD.bshutDOff\
    -in $tab.tshutD\
    -row 1\
    -column 1\
    -padx 5\
    -pady 5


grid $tab.tshutD.tshutDtxt\
    -in $tab.tshutD\
    -row 1\
    -column 2\
    -padx 5\
    -pady 5

grid $tab.tshutD.tshutDvar\
    -in $tab.tshutD\
    -row 1\
    -column 3\
    -padx 5\
    -pady 5

grid $tab.tlampA.blampAOn\
    -in $tab.tlampA\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5

grid $tab.tlampA.blampAOff\
    -in $tab.tlampA\
    -row 1\
    -column 1\
    -padx 5\
    -pady 5

grid $tab.tlampA.tlampAtxt\
    -in $tab.tlampA\
    -row 1\
    -column 2\
    -padx 5\
    -pady 5

grid $tab.tlampA.tlampAvar\
    -in $tab.tlampA\
    -row 1\
    -column 3\
    -padx 5\
    -pady 5

grid $tab.tlampB.tlampBtxt\
    -in $tab.tlampB\
    -row 1\
    -column 2\
    -padx 5\
    -pady 5

grid $tab.tlampB.tlampBvar\
    -in $tab.tlampB\
    -row 1\
    -column 3\
    -padx 5\
    -pady 5



grid $tab.tlampB.blampBOn\
    -in $tab.tlampB\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5

grid $tab.tlampB.blampBOff\
    -in $tab.tlampB\
    -row 1\
    -column 1\
    -padx 5\
    -pady 5



grid $tab.tshutA\
    -in $tab\
    -row 0\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew

grid $tab.tshutB\
    -in $tab\
    -row 1\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew

grid $tab.tshutC\
    -in $tab\
    -row 2\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew

grid $tab.tshutD\
    -in $tab\
    -row 3\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew

grid $tab.tlampA\
    -in $tab\
    -row 5\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew

grid $tab.tlampB\
    -in $tab\
    -row 6\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew

grid $tab.tsensor\
    -in $tab\
    -row 7\
    -column 0\
    -padx 5\
    -pady 5\
    -sticky ew


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Opens 
"


# -----------------------------
# Function
# -----------------------------
proc onfunction { ttlid } {
    variable commandID 80
    variable commandIDOn $commandID
    variable commandName "TTL On"
    variable commandQuestion "Do you want to turn TTL $ttlid ON?"
    variable commandMessage "$commandID: TTL $ttlid ON"
    variable commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandIDOn $ttlid]
	message $mbody $commandMessage $commandLog
    } else {

    }
}

proc offfunction { ttlid } {
    variable commandID 81
    variable commandIDOff 81
    variable commandName "TTL Off"
    variable commandQuestion "Do you want to turn TTL $ttlid OFF?"
    variable commandMessage "$commandID: TTL $ttlid OFF"
    variable commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandIDOff $ttlid]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns
}

