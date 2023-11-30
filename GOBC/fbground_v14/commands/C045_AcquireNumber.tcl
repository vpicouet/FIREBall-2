set ns "C045"
namespace eval $ns {
variable commandID 45


# -----------------------------
# pathname for the controls
# -----------------------------
set frame  $positions::tGuiderMode.fSetGuiderMode

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Acquire a specific number of stars.
"


variable starcount 1


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Acquire specific number of stars"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Acquire N stars?
N = $starcount\n
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
    variable starcount

    if { $starcount < 1 } { set starcount 1 }

    set starcount [expr { $starcount & 0xFF } ]

set commandQuestion \
"Do you want to acquire N stars
N = $starcount\n
"   

    set commandMessage "\[$commandID] $commandName starcount=$starcount"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID [expr {0xFF & $starcount}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


iwidgets::regexpfield $frame.starcount \
    -regexp {^[0-9]+$}\
    -labeltext "maxstars:"\
    -textvariable "${ns}::starcount" \
    -width 8


button $frame.bStarCount \
    -text "NAcquire" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $frame.bStarCount\
    -in $frame\
    -row 3\
    -column 1\
    -columnspan 1

grid $frame.starcount\
    -in $frame\
    -row 3\
    -column 0\
    -columnspan 1
# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

