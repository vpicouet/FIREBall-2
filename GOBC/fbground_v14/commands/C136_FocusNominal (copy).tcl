set ns "C133"
namespace eval $ns {
variable commandID 133


# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::FocusNominalTab
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
-------------------------------
Goes to nominal focus.
"

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "ABC moves"
variable commandQuestion "Do you want to start ABC moves?"
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

ttk::labelframe $tab.fFocusNominal \
    -borderwidth 2 \
    -relief ridge \
    -text "Nominal Focus"


if { $positions::P136On } {
    grid $tab.fFocusNominal\
	-in $tab\
	-row $positions::FocusNominalY\
	-column $positions::FocusNominalX\
	-pady $positions::padY
}

button $tab.fFocusNominal.bFocusNominal \
    -text "ABC" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"

#pack $tab.fRotaryStage.bRotaryHome
grid $tab.fFocusNominal.bFocusNominal\
    -in $tab.fFocusNominal\
    -row 0\
    -column 0\
    -padx $positions::padX\
    -pady $positions::padY



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fFocusNominal.bFocusNominal $tthelp


}

