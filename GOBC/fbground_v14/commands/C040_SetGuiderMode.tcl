set ns "C040"
namespace eval $ns {
variable commandID 40


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tGuiderMode

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Set the Guider Mode.
"

variable guidermode 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set guider mode"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to set the guider mode
to $guidermode?
"

# -----------------------------
# Function
# -----------------------------
proc myfunction { guidmode } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable guidermode

    set guidermode $guidmode
    if { $guidmode < 0 } { set guidermode 0  }
    if { $guidmode > 3 } { set guidermode 3  }

set commandQuestion \
"
Do you want to set the guidermode mode
to $guidermode?
"   

    set commandMessage "\[$commandID] $commandName $guidermode"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID $guidermode]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fSetGuiderMode \
    -borderwidth 2 \
    -relief ridge \
    -text "Set GuiderMode Mode"

pack $tab.fSetGuiderMode

button $tab.fSetGuiderMode.bGuidemodeIdle \
    -text "Idle <0>" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 0"\
    -width 15

button $tab.fSetGuiderMode.bGuidemodeAcquire \
    -text "Acquire <1>" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 1"\
    -width 15

button $tab.fSetGuiderMode.bGuidemodeMSFixed \
    -text "Multi-Fixed <2>" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 2"\
    -width 15

button $tab.fSetGuiderMode.bGuidemodeMSFollow \
    -text "Multi-Follow <3>" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 3"\
    -width 15

grid $tab.fSetGuiderMode.bGuidemodeIdle\
    -in $tab.fSetGuiderMode\
    -row 0\
    -column 0

grid $tab.fSetGuiderMode.bGuidemodeAcquire\
    -in $tab.fSetGuiderMode\
    -row 0\
    -column 1

grid $tab.fSetGuiderMode.bGuidemodeMSFixed\
    -in $tab.fSetGuiderMode\
    -row 1\
    -column 0

grid $tab.fSetGuiderMode.bGuidemodeMSFollow\
    -in $tab.fSetGuiderMode\
    -row 1\
    -column 1

#grid $tab.fSetExpTime.exptime\
#    -in $tab.fSetExpTime\
#    -row 2\
#    -column 0\
#    -columnspan 2
# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetGuiderMode.bExpTime $tthelp


}

