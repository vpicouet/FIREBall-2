set ns "C020"
namespace eval $ns {
variable commandID 20


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tDisplayStretch

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Set display stretc
"


variable mn 0
variable mx 65535
variable gamma 1.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set the display stretch"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to set the display stretch to:
min = $mn
max = $mx
gamma = $gamma
"

# -----------------------------
# Function
# -----------------------------
proc myfunction { rst } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable mn
    variable mx
    variable gamma




    if { $rst != 0 } {
	set commandQuestion \
	    "Do you want to reset the stretch?"
	set mn -1
    } else { 
	set commandQuestion \
	    "Do you want to set the display stretch to:
             min = $mn
             max = $mx 
             gamma = $gamma"
    }

    set commandMessage "\[$commandID] $commandName min=$mn max=$mx gamma=$gamma"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ciif $commandID $mn $mx $gamma]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fDisplayStretch \
    -borderwidth 2 \
    -relief ridge \
    -text "Set Display Stretch"

pack $tab.fDisplayStretch

iwidgets::regexpfield $tab.fDisplayStretch.mn \
    -regexp {^[0-9]+$}\
    -labeltext "Min:"\
    -textvariable "${ns}::mn" \
    -width 8

iwidgets::regexpfield $tab.fDisplayStretch.mx \
    -regexp {^[0-9]+$}\
    -labeltext "Max:"\
    -textvariable "${ns}::mx" \
    -width 8

iwidgets::regexpfield $tab.fDisplayStretch.gamma \
    -regexp {^[0-9]+$}\
    -labeltext "gamma:"\
    -textvariable "${ns}::gamma" \
    -width 8

button $tab.fDisplayStretch.bDisplayStretch \
    -text "Set Disp. Stretch" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 0"\
    -width 15

button $tab.fDisplayStretch.bResetStretch \
    -text "Reset Disp. Stretch" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 1"\
    -width 25


grid $tab.fDisplayStretch.mn\
    -in $tab.fDisplayStretch\
    -row 0 \
    -column 0\
    -columnspan 1

grid $tab.fDisplayStretch.mx\
    -in $tab.fDisplayStretch\
    -row 0 \
    -column 1\
    -columnspan 1

grid $tab.fDisplayStretch.gamma\
    -in $tab.fDisplayStretch\
    -row 1 \
    -column 0\
    -columnspan 1

grid $tab.fDisplayStretch.bDisplayStretch\
    -in $tab.fDisplayStretch\
    -row 1\
    -column 1

grid $tab.fDisplayStretch.bResetStretch\
    -in $tab.fDisplayStretch\
    -row 2\
    -column 0\
    -columnspan 2


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

