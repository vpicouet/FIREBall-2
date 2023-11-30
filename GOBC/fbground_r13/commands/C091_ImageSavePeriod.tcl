set ns "C091"
namespace eval $ns {
variable commandID 91


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tSavePeriod

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Change save period to every N frames.  
0 -- turns off
"


variable target 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change save period"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change the save period?
0 -- turns off
saveperiod = $target\n
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
    variable target

    set target [expr { $target & 0xFFFF } ]

set commandQuestion \
"Do you want to change save period?
0=turn off
target = $target\n
"   

    set commandMessage "\[$commandID] $commandName target=$target"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cs $commandID [expr {0xFFFF & $target}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fChangeSavePeriod \
    -borderwidth 2 \
    -relief ridge \
    -text "Save Period"

pack $tab.fChangeSavePeriod

iwidgets::regexpfield $tab.fChangeSavePeriod.target \
    -regexp {^[0-9]+$}\
    -labeltext "SavePeriod:"\
    -textvariable "${ns}::target" \
    -width 8


button $tab.fChangeSavePeriod.bChangeSavePeriod \
    -text "Change SavePeriod" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15


ttk::label $tab.fChangeSavePeriod.value \
    -textvariable "var::saveperiod" \
    -width 8 \
    -justify right\
    -foreground blue


#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fChangeSavePeriod.bChangeSavePeriod\
    -in $tab.fChangeSavePeriod\
    -row 2\
    -column 2

grid $tab.fChangeSavePeriod.target\
    -in $tab.fChangeSavePeriod\
    -row 2\
    -column 0\
    -columnspan 2


grid $tab.fChangeSavePeriod.value\
    -in $tab.fChangeSavePeriod\
    -row 2\
    -column 5\
    -columnspan 2

# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

