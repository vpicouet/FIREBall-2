set ns "C041"
namespace eval $ns {
variable commandID 41


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tNTtoS

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Convert next target coordinates to next star coordinates
"


variable dx 0.0
variable dy 0.0
variable dr 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Set Next Target to Next Star"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to convert next target to next star via:
dx = $dx
dy = $dy 
dr = $dr (radians)
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
    variable dx
    variable dy
    variable dr


set commandQuestion \
"
Do you want to convert next target to next star via:
dx = $dx
dy = $dy 
dr = $dr (radians)
"   

    set commandMessage "\[$commandID] $commandName dx=$dx dy=$dy dr=$dr"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cfff $commandID $dx $dy $dr]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fNTtoS \
    -borderwidth 2 \
    -relief ridge \
    -text "Next Target --> Next Star"

pack $tab.fNTtoS

iwidgets::regexpfield $tab.fNTtoS.dx \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "dx (pix):"\
    -textvariable "${ns}::dx" \
    -width 6

iwidgets::regexpfield $tab.fNTtoS.dy \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "dy (pix):"\
    -textvariable "${ns}::dy" \
    -width 6

iwidgets::regexpfield $tab.fNTtoS.dr \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "dr (rad):"\
    -textvariable "${ns}::dr" \
    -width 6

button $tab.fNTtoS.bNTtoS \
    -text "Tgt-->Star" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 12

grid $tab.fNTtoS.dx\
    -in $tab.fNTtoS\
    -row 0 \
    -column 0\
    -columnspan 1

grid $tab.fNTtoS.dy\
    -in $tab.fNTtoS\
    -row 0 \
    -column 1\
    -columnspan 1

grid $tab.fNTtoS.dr\
    -in $tab.fNTtoS\
    -row 0 \
    -column 2\
    -columnspan 1

grid $tab.fNTtoS.bNTtoS\
    -in $tab.fNTtoS\
    -row 0\
    -column 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

