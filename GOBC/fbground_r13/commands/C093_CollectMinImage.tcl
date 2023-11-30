set ns "C093"
namespace eval $ns {
variable commandID 93


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tCollectMin
set suffix RefineTargets

ttk::labelframe $tab.fCollectMin \
    -borderwidth 2 \
    -relief ridge \
    -text "Collect Min Image"

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Refine image subtraction using N targets
"


variable pos 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Collect Min Image"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to collect 
$pos images to refine background?
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
"Do you want to refine background
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


iwidgets::regexpfield $tab.fCollectMin.pos \
    -regexp {^[0-9]+$}\
    -labeltext "Samples:"\
    -textvariable "${ns}::pos" \
    -width 8


button $tab.fCollectMin.b$suffix \
    -text "Background!" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fLinearStage1.b$suffix \
#    -side right
#pack $tab.fLinearStage1.pos\
#    -fill x \
#    -side left\
#    -expand 1

pack $tab.fCollectMin
#    -in $tab\
#    -row 5\
#    -column 0

grid $tab.fCollectMin.b$suffix \
    -in $tab.fCollectMin\
    -row 1\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fCollectMin.pos\
    -in $tab.fCollectMin\
    -row 1\
    -column 0\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fCollectMin.b$suffix $tthelp


}

