set ns "C130"
namespace eval $ns {
variable commandID 130


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::LinearStageTab3
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
variable commandName "Home Linear 3"
variable commandQuestion "Do you want to home the linear stages 3?"
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

ttk::labelframe $tab.fLinearStage3 \
    -borderwidth 2 \
    -relief ridge \
    -text "Linear Stage 3"


if { $positions::P110On } {
    grid $tab.fLinearStage3\
	-in $tab\
	-row $positions::LinearStage3Y\
	-columnspan 2\
	-sticky ew\
	-column $positions::LinearStage3X\
	-padx $positions::padX\
	-pady $positions::padY
    
}

button $tab.fLinearStage3.bLinearHome \
    -text "Home Linear C" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"

# pack $tab.fLinearStage3.bLinearHome

grid $tab.fLinearStage3.bLinearHome\
    -in $tab.fLinearStage3\
    -row 0\
    -column 0\
    -padx $positions::padX\
    -pady $positions::padY

# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLinearStage3.bLinearHome $tthelp


}

