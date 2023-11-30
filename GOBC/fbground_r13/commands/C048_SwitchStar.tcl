set ns "C048"
namespace eval $ns {
variable commandID 48


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tSwitchStar

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Switch guiding to a different star
"


variable x 0.0
variable y 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Switch guiding to a different star"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to switch guiding to a different star
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
    variable x
    variable y


set commandQuestion \
"
Do you want to switch guiding to a different star
x = $x
y = $y 
"   

    set commandMessage "\[$commandID] $commandName x=$x y=$y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cff $commandID $x $y]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fSwitchStar \
    -borderwidth 2 \
    -relief ridge \
    -text "Switch star"

pack $tab.fSwitchStar

iwidgets::regexpfield $tab.fSwitchStar.x \
    -labeltext "x (pix):"\
    -textvariable "${ns}::x" \
    -width 8

iwidgets::regexpfield $tab.fSwitchStar.y \
    -labeltext "y (pix):"\
    -textvariable "${ns}::y" \
    -width 8

button $tab.fSwitchStar.bSwitchStar \
    -text "Switch Star" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 20

grid $tab.fSwitchStar.x\
    -in $tab.fSwitchStar\
    -row 0 \
    -column 0\

grid $tab.fSwitchStar.y\
    -in $tab.fSwitchStar\
    -row 0 \
    -column 1\

grid $tab.fSwitchStar.bSwitchStar\
    -in $tab.fSwitchStar\
    -row 0\
    -column 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

