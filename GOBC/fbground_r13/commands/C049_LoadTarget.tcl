set ns "C049"
namespace eval $ns {
variable commandID 49


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tLoadTarget

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Load prepared target points
"


variable target 0
variable loadstar 0 


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Load next target"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to load target field:
target = $target\n
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
    variable loadstar

    if { $target < 0 } { set target 1 }

    set target [expr { $target & 0xFFFF } ]

set commandQuestion \
"Do you want to laod target field:
target = $target\n
load star = $loadstar\n
"   

    set commandMessage "\[$commandID] $commandName target=$target; load star=$loadstar"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format csu $commandID [expr {0xFFFF & ( $target + 1000 * $loadstar) } ] ] 
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fLoadTarget \
    -borderwidth 2 \
    -relief ridge \
    -text "LoadTarget"

pack $tab.fLoadTarget

iwidgets::regexpfield $tab.fLoadTarget.target \
    -regexp {^[0-9]+$}\
    -labeltext "Target ID:"\
    -textvariable "${ns}::target" \
    -width 8

ttk::checkbutton $tab.fLoadTarget.cLoadStar \
    -offvalue 0 \
    -onvalue 1 \
    -variable "${ns}::loadstar" \
    -text "Load stars too?"

button $tab.fLoadTarget.bLoadTarget \
    -text "Load Target" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fLoadTarget.bLoadTarget\
    -in $tab.fLoadTarget\
    -row 1\
    -column 2

grid $tab.fLoadTarget.target\
    -in $tab.fLoadTarget\
    -row 1\
    -column 0\
    -columnspan 2

grid $tab.fLoadTarget.cLoadStar\
    -in $tab.fLoadTarget\
    -row 1\
    -column 3
# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

