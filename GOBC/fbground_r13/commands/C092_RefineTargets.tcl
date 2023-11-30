set ns "C092"
namespace eval $ns {
variable commandID 92


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tRefineTargets
set suffix RefineTargets

ttk::labelframe $tab.fRefineTargets \
    -borderwidth 2 \
    -relief ridge \
    -text "Refine Targets"

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Refine targets using N samples
"


variable pos 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Refine Targets"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to send position:
delta pos = $pos mm
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
    variable pos

    if { $pos < 10} { set pos 10 }
    if { $pos > 255 } { set pos 255 }

set commandQuestion \
"Do you want to refine targets
using $pos samples?"   

    set commandMessage "\[$commandID] $commandName pos = $pos"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID [expr $pos & 0xff] ]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


iwidgets::regexpfield $tab.fRefineTargets.pos \
    -regexp {^[0-9]+$}\
    -labeltext "Samples:"\
    -textvariable "${ns}::pos" \
    -width 8


button $tab.fRefineTargets.b$suffix \
    -text "Refine!" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fLinearStage1.b$suffix \
#    -side right
#pack $tab.fLinearStage1.pos\
#    -fill x \
#    -side left\
#    -expand 1

pack $tab.fRefineTargets
#    -in $tab\
#    -row 5\
#    -column 0

grid $tab.fRefineTargets.b$suffix \
    -in $tab.fRefineTargets\
    -row 1\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fRefineTargets.pos\
    -in $tab.fRefineTargets\
    -row 1\
    -column 0\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fRefineTargets.b$suffix $tthelp


}

