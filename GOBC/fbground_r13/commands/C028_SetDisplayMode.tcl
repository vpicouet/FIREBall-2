set ns "C028"
namespace eval $ns {
variable commandID 28


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tDisplayMode

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Set the Display Mode.
"

variable displaymode 0

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set display mode"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to set the display mode
to $displaymode?
"

# -----------------------------
# Function
# -----------------------------
proc myfunction { dispmode } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable displaymode

    set displaymode $dispmode
    if { $dispmode < 0 } { set displaymode 0  }
    if { $dispmode > 1 } { set displaymode 1  }

set commandQuestion \
"
Do you want to set the display mode
to $displaymode?
"   

    set commandMessage "\[$commandID] $commandName $displaymode"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID $displaymode]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fSetDisplayMode \
    -borderwidth 2 \
    -relief ridge \
    -text "Set DisplayMode Mode"

pack $tab.fSetDisplayMode

button $tab.fSetDisplayMode.bDisplaymodeFull \
    -text "Full < 0 >" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 0"\
    -width 15

button $tab.fSetDisplayMode.bDisplaymodeSmall \
    -text "Small < 1 >" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 1"\
    -width 15

grid $tab.fSetDisplayMode.bDisplaymodeFull \
    -in $tab.fSetDisplayMode\
    -row 0\
    -column 0

grid $tab.fSetDisplayMode.bDisplaymodeSmall \
    -in $tab.fSetDisplayMode\
    -row 0\
    -column 1


}

