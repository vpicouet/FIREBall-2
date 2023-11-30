set ns "C004"
namespace eval $ns {
variable commandID 4


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tExposureTime

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Set the exposure time (in ms)
"


variable exptime 100


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set exposure time"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to request exposure time
exptime = $exptime ms?
"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable exptime

    if { $exptime < 1 } { set exptime 1 }
    if { $exptime > 3000 } { set exptime 3000 }

set commandQuestion \
"Do you want to request exptime:
exptime = $exptime ms
"   

    set commandMessage "\[$commandID] $commandName exptime = $exptime ms"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ci $commandID [expr {0xFFFFFFFF & $exptime}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fSetExpTime \
    -borderwidth 2 \
    -relief ridge \
    -text "Set Exposure Time"

pack $tab.fSetExpTime

iwidgets::regexpfield $tab.fSetExpTime.exptime \
    -regexp {^[0-9]+$}\
    -labeltext "exptime (ms):"\
    -textvariable "${ns}::exptime" \
    -width 8


button $tab.fSetExpTime.bExpTime \
    -text "Exposure Time" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fSetExpTime.bExpTime\
    -in $tab.fSetExpTime\
    -row 2\
    -column 2

grid $tab.fSetExpTime.exptime\
    -in $tab.fSetExpTime\
    -row 2\
    -column 0\
    -columnspan 2
# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

