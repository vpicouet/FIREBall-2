set ns "C120"
namespace eval $ns {
variable commandID 120


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::LinearStageTab2
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
-------------------------------
Homes the linear stage
"

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Home Linear 2"
variable commandQuestion "Do you want to home the linear stages 2?"
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

ttk::labelframe $tab.fLinearStage2 \
    -borderwidth 2 \
    -relief ridge \
    -text "Linear Stage 2"


if { $positions::P110On } {
    grid $tab.fLinearStage2\
	-in $tab\
	-row $positions::LinearStage2Y\
	-columnspan 2\
	-sticky ew\
	-column $positions::LinearStage2X\
	-padx $positions::padX\
	-pady $positions::padY
    
}

button $tab.fLinearStage2.bLinearHome \
    -text "Home Linear B" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"

# pack $tab.fLinearStage2.bLinearHome

grid $tab.fLinearStage2.bLinearHome\
    -in $tab.fLinearStage2\
    -row 0\
    -column 0\
    -padx $positions::padX\
    -pady $positions::padY

# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLinearStage2.bLinearHome $tthelp


}

