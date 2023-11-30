set ns "C132"
namespace eval $ns {
variable commandID 132

variable focus_delay 15
variable focus_step 0.25
variable det 0
# -----------------------------
# pathname for the controls
# -----------------------------
variable tab $positions::FocusTipTiltTab
    proc setLocation {loc} {
	variable tab
	set tab loc
    }



# -----------------------------
# Help Variables
# -----------------------------
set tthelp "Command ID $commandID
-------------------------------
Executes a focus sequence.
"

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Focus TipTilt"
variable commandQuestion "Do you want to run focus command:"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage

# -----------------------------
# Function
# -----------------------------
proc myfunction { mymid } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable focus_delay
    variable focus_step
    variable det 
    if { $mymid == 132 } {
	set commandName "Focus TipTilt"
	set commandMessage "\[$mymid] $commandName"
	set commandLog $commandName
	if { $focus_step < 0.1 } { 
	    set focus_step 0.1
	}
	if { $focus_step > 1.5 } {
	    set focus_step 1.5
	}
	if { $focus_delay < 5 } { set focus_delay 5 }
	if { $focus_delay > 127 } { set focus_delay 127 }
	set delay $focus_delay
	if { $det > 0 } { set delay [expr $focus_delay * (-1) ]}
	set det 0

	if { [verify "$commandQuestion $mymid"]} {
	    set mbody [binary format ccf [ expr $mymid & 0xff ] [expr $delay & 0xff ] $focus_step ]
	    message $mbody $commandMessage $commandLog
	}
    } else { 
	set commandName "Nominal Focus"
	
	set commandMessage "\[$mymid] $commandName"
	set commandLog $commandName
	
	if { [verify "$commandQuestion $mymid"]} {
	    set mbody [binary format c [ expr $mymid & 0xff ] ]
	    message $mbody $commandMessage $commandLog
	}
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

# First we make a frame.
# ----------------------------

ttk::labelframe $tab.fFocusTipTilt \
    -borderwidth 2 \
    -relief ridge \
    -text "Focus"


if { $positions::P132On } {
    grid $tab.fFocusTipTilt\
	-in $tab\
	-row $positions::FocusTipTiltY\
	-column $positions::FocusTipTiltX\
	-pady $positions::padY
}

button $tab.fFocusTipTilt.bFocusTipTilt \
    -text "Focus" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 132"

button $tab.fFocusTipTilt.bFocusNominal \
    -text "Nominal" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 136"

iwidgets::regexpfield $tab.fFocusTipTilt.fd \
    -labeltext "Del(s):"\
    -textvariable "${ns}::focus_delay" \
    -fixed 8\
    -regexp {^[0-9]+$}\
    -width 6

iwidgets::regexpfield $tab.fFocusTipTilt.fs \
    -labeltext "Step(mm):"\
    -textvariable "${ns}::focus_step" \
    -fixed 8\
    -regexp {^[\.0-9]+$}\
    -width 6

ttk::checkbutton $tab.fFocusTipTilt.det\
    -offvalue 0 \
    -onvalue 1 \
    -variable "${ns}::det"\
    -text "Det"

#pack $tab.fRotaryStage.bRotaryHome
grid $tab.fFocusTipTilt.fd\
    -in $tab.fFocusTipTilt\
    -row 0\
    -column 0\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fFocusTipTilt.fs\
    -in $tab.fFocusTipTilt\
    -row 0\
    -column 1\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fFocusTipTilt.bFocusTipTilt\
    -in $tab.fFocusTipTilt\
    -row 0\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

#pack $tab.fRotaryStage.bRotaryHome
grid $tab.fFocusTipTilt.bFocusNominal\
    -in $tab.fFocusTipTilt\
    -row 0\
    -column 3\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fFocusTipTilt.det\
    -in $tab.fFocusTipTilt\
    -row 0\
    -column 4


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fFocusTipTilt.bFocusTipTilt $tthelp


}

