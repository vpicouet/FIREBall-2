set ns "C100"
namespace eval $ns {
variable commandID 100


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::RotaryStageTab
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
-------------------------------
Homes the rotary stage
"

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Home Rotary"
variable commandQuestion "Do you want to home the rotary stage?"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    if { [verify "$commandQuestion"]} {
	set mbody [binary format c $commandID]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

# First we make a frame.
# ----------------------------

ttk::labelframe $tab.fRotaryStage \
    -borderwidth 2 \
    -relief ridge \
    -text "Rotary Stage"


if { $positions::P100On } {
    grid $tab.fRotaryStage\
	-in $tab\
	-row $positions::RotaryStageY\
	-column $positions::RotaryStageX\
	-padx $positions::padX\
	-pady $positions::padY
}

button $tab.fRotaryStage.bRotaryHome \
    -text "Home Rotary" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"

#pack $tab.fRotaryStage.bRotaryHome
grid $tab.fRotaryStage.bRotaryHome\
    -in $tab.fRotaryStage\
    -row 0\
    -column 0\
    -padx $positions::padX\
    -pady $positions::padY



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fRotaryStage.bRotaryHome $tthelp


}

