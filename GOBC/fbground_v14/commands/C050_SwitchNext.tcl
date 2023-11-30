set ns "C050"
namespace eval $ns {
variable commandID 50


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tSwitchNext

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Switch to next target, star, or both
"


variable next 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Switch to Next"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to switch to next
 target, star, or both?
"

# -----------------------------
# Function
# -----------------------------
proc myfunction { nex } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable next
    
    set next $nex
    if { $next < 0 } { set next 0 }
    if { $next > 2 } { set next 2 } 
    set next [expr { $next & 0xFF } ]

set commandQuestion \
"Switch to next target, star, or both < $next >?"
   

    set commandMessage "\[$commandID] $commandName $next"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccu $commandID [expr {0xFF & $next}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fSwitchNext \
    -borderwidth 2 \
    -relief ridge \
    -text "Switch to Next"

pack $tab.fSwitchNext

button $tab.fSwitchNext.bSwitchNextTarget \
    -text "Next Target < 0 >" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 0"\
    -width 15

button $tab.fSwitchNext.bSwitchNextStar \
    -text "Next Star < 1 >" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 1"\
    -width 15

button $tab.fSwitchNext.bSwitchNextBoth \
    -text "Next Both < 2 >" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 2"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fSwitchNext.bSwitchNextTarget\
    -in $tab.fSwitchNext\
    -row 0\
    -column 0

grid $tab.fSwitchNext.bSwitchNextStar\
    -in $tab.fSwitchNext\
    -row 0\
    -column 1

grid $tab.fSwitchNext.bSwitchNextBoth\
    -in $tab.fSwitchNext\
    -row 0\
    -column 2


# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

