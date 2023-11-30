set ns "C110"
namespace eval $ns {
variable commandID 110


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::LinearStageTab1
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
variable commandName "Home Linear 1"
variable commandQuestion "Do you want to home linear stage 1?"
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

ttk::labelframe $tab.fLinearStage1 \
    -borderwidth 2 \
    -relief ridge \
    -text "Linear Stage 1"


if { $positions::P110On } {
    grid $tab.fLinearStage1\
	-in $tab\
	-row $positions::LinearStage1Y\
	-columnspan 2\
	-sticky ew\
	-column $positions::LinearStage1X\
	-padx $positions::padX\
	-pady $positions::padY
    
}

button $tab.fLinearStage1.bLinearHome \
    -text "Home Linear A" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"

# pack $tab.fLinearStage1.bLinearHome

grid $tab.fLinearStage1.bLinearHome\
    -in $tab.fLinearStage1\
    -row 0\
    -column 0\
    -padx $positions::padX\
    -pady $positions::padY

# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLinearStage1.bLinearHome $tthelp


}

