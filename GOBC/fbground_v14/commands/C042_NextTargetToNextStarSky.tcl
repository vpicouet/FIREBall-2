set ns "C042"
namespace eval $ns {
variable commandID 42


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tNTtoSS

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Convert next target coordinates to next star coordinates
via astrometry
"



# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set Next Target to Next Star via Astrometry"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to convert next target to next star via
astrometry
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

set commandQuestion \
"
Do you want to convert next target to next star via:
astrometry
"   

    set commandMessage "\[$commandID] $commandName"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format c $commandID ]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fNTtoSS \
    -borderwidth 2 \
    -relief ridge \
    -text "Next Target --Astrometry--> Next Star"

pack $tab.fNTtoSS

button $tab.fNTtoSS.bNTtoSS \
    -text "Target --Astrometry--> Star" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 30

grid $tab.fNTtoSS.bNTtoSS\
    -in $tab.fNTtoSS\
    -row 1\
    -column 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

