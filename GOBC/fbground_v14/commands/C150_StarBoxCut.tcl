set ns "C150"
namespace eval $ns {
variable commandID 150


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tStarBoxCut

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Change star box cut dimensions.
"


variable starboxcut 5


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change star box cut"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Change star box cut to 
size = $starboxcut\n
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
    variable starboxcut

    if { $starboxcut < 1 } { set starboxcut 1 }

    set starboxcut [expr { $starboxcut & 0xFF } ]

set commandQuestion \
"Do you want to change star cut to: 
starcut = $starboxcut\n
"   

    set commandMessage "\[$commandID] $commandName starbox=$starboxcut"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID [expr {0xFF & $starboxcut}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fStarBoxCut \
    -borderwidth 2 \
    -relief ridge \
    -text "Star Box Cut"

pack $tab.fStarBoxCut

iwidgets::regexpfield $tab.fStarBoxCut.starboxcut \
    -regexp {^[0-9]+$}\
    -labeltext "Starbox:"\
    -textvariable "${ns}::starboxcut" \
    -width 8


button $tab.fStarBoxCut.bStarBoxCut \
    -text "Set star box" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fStarBoxCut.bStarBoxCut\
    -in $tab.fStarBoxCut\
    -row 2\
    -column 2

grid $tab.fStarBoxCut.starboxcut\
    -in $tab.fStarBoxCut\
    -row 2\
    -column 0\
    -columnspan 2
# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

