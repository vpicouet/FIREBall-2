set ns "C046"
namespace eval $ns {
variable commandID 46


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tAdjustStar

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Adjust pointing by moving single star
"


variable starid 0
variable x 100.0
variable y 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Adjust Pointing Single Star"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to adjust to:
ID = $starid
x = $x
y = $y
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
    variable starid
    variable x
    variable y


set commandQuestion \
"
Do you want to adjust the pointing to:
id = $starid
x = $x
y = $y
"   

    set commandMessage "\[$commandID] $commandName id=$starid x=$x y=$y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccff $commandID $starid $x $y]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fAdjustStar \
    -borderwidth 2 \
    -relief ridge \
    -text "Adjust One Star"

pack $tab.fAdjustStar

iwidgets::regexpfield $tab.fAdjustStar.starid \
    -labeltext "starID:"\
    -textvariable "${ns}::starid" \
    -width 6

iwidgets::regexpfield $tab.fAdjustStar.x \
    -labeltext "x (pix):"\
    -textvariable "${ns}::x" \
    -width 6

iwidgets::regexpfield $tab.fAdjustStar.y \
    -labeltext "y (pix):"\
    -textvariable "${ns}::y" \
    -width 6

button $tab.fAdjustStar.bAdjustStar \
    -text "Adj. Star" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 12

grid $tab.fAdjustStar.starid\
    -in $tab.fAdjustStar\
    -row 0 \
    -column 0\
    -columnspan 1

grid $tab.fAdjustStar.x\
    -in $tab.fAdjustStar\
    -row 0 \
    -column 1\
    -columnspan 1

grid $tab.fAdjustStar.y\
    -in $tab.fAdjustStar\
    -row 0 \
    -column 2\
    -columnspan 1

grid $tab.fAdjustStar.bAdjustStar\
    -in $tab.fAdjustStar\
    -row 0\
    -column 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

