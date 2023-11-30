set ns "C044"
namespace eval $ns {
variable commandID 44


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tAdjustPointing

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Adjust pointing
"


variable dx 0.0
variable dy 0.0
variable dr 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Adjust Pointing"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to adjust the pointing by:
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
Do you want to adjust the pointing by:
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


ttk::labelframe $tab.fAdjustPointing \
    -borderwidth 2 \
    -relief ridge \
    -text "Adjust Pointing"

pack $tab.fAdjustPointing

iwidgets::regexpfield $tab.fAdjustPointing.dx \
    -labeltext "dx (pix):"\
    -textvariable "${ns}::dx" \
    -width 6

iwidgets::regexpfield $tab.fAdjustPointing.dy \
    -labeltext "dy (pix):"\
    -textvariable "${ns}::dy" \
    -width 6

iwidgets::regexpfield $tab.fAdjustPointing.dr \
    -labeltext "dr (rad):"\
    -textvariable "${ns}::dr" \
    -width 6

button $tab.fAdjustPointing.bAdjustPointing \
    -text "Adj. Pointing" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 12

grid $tab.fAdjustPointing.dx\
    -in $tab.fAdjustPointing\
    -row 0 \
    -column 0\
    -columnspan 1

grid $tab.fAdjustPointing.dy\
    -in $tab.fAdjustPointing\
    -row 0 \
    -column 1\
    -columnspan 1

grid $tab.fAdjustPointing.dr\
    -in $tab.fAdjustPointing\
    -row 0 \
    -column 2\
    -columnspan 1

grid $tab.fAdjustPointing.bAdjustPointing\
    -in $tab.fAdjustPointing\
    -row 0\
    -column 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

