set ns "C090"
namespace eval $ns {
variable commandID 90


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tSetImageTag

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Tag the saved images and image headers with a number
"


variable target 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change image header tag"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change the image tag?
-- 0 means increment --
imagetag = $target\n
"

# -----------------------------
# Function
# -----------------------------
proc myfunction { targ } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable target

#    set target [expr { $targ & 0xFFFF } ]

set commandQuestion \
"Do you want to change the image tag?
(0 means increment)
imagetag = $targ\n
"   

    set commandMessage "\[$commandID] $commandName imagetag=$targ"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cs $commandID [expr {0xFFFF & $targ}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fSetImageTag \
    -borderwidth 2 \
    -relief ridge \
    -text "Image Tag"

grid $tab.fSetImageTag \
    -in $tab\
    -row 3\
    -column 0


iwidgets::regexpfield $tab.fSetImageTag.target \
    -regexp {^[0-9]+$}\
    -labeltext "Image tag:"\
    -textvariable "${ns}::target" \
    -width 8

ttk::label $tab.fSetImageTag.value \
    -textvariable "var::imagetag" \
    -width 8 \
    -justify right\
    -foreground blue


button $tab.fSetImageTag.bSetImageTag \
    -text "Set Tag" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction $${ns}::target" \
    -width 15

button $tab.fSetImageTag.bIncrementImageTag \
    -text "Increment Tag" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 0" \
    -width 15

grid $tab.fSetImageTag.bSetImageTag\
    -in $tab.fSetImageTag\
    -row 2\
    -column 2

grid $tab.fSetImageTag.bIncrementImageTag\
    -in $tab.fSetImageTag\
    -row 3\
    -column 2


grid $tab.fSetImageTag.target\
    -in $tab.fSetImageTag\
    -row 2\
    -column 0\
    -columnspan 2

grid $tab.fSetImageTag.value\
    -in $tab.fSetImageTag\
    -row 2\
    -column 5\
    -columnspan 2
# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

