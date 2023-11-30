set ns "C101"
namespace eval $ns {
variable commandID 101


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::RotaryStageTab
set suffix RotaryPos

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Command an absolute
rotary stage position in
degree
"


variable angle 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Rotary stage absolute"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to send offsets:
ang = $angle degrees
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
    variable angle

    if { $angle < -360 } { set angle -360 }
    if { $angle > 360 } { set angle 360 }

set commandQuestion \
"Do you want to send rotary
stage angle:
angle = $angle degrees
"   

    set commandMessage "\[$commandID] $commandName angle = $angle"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cf $commandID $angle]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


iwidgets::regexpfield $tab.fRotaryStage.angle \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "angle (deg):"\
    -textvariable "${ns}::angle" \
    -width 8


button $tab.fRotaryStage.b$suffix \
    -text "Abs. Angle" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fRotaryStage.b$suffix\
    -in $tab.fRotaryStage\
    -row 2\
    -column 2

grid $tab.fRotaryStage.angle\
    -in $tab.fRotaryStage\
    -row 2\
    -column 0\
    -columnspan 2
# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fRotaryStage.b$suffix $tthelp


}

