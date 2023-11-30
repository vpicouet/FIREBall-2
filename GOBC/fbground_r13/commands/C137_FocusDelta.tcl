set ns "C137"
namespace eval $ns {
variable commandID 137


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::LinearDelta
set suffix LinearDelta

ttk::labelframe $tab.fLinearDelta \
    -borderwidth 2 \
    -relief ridge \
    -text "Linear Focus Delta"

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Command a delta move on 
linear stage position in
mm
"


variable pos 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Linear stage delta"
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

    if { $pos < -26 } { set pos -26 }
    if { $pos > 26 } { set pos 26 }

set commandQuestion \
"Do you want to send linear 
delta stage pos:
delta pos = $pos mm
"   

    set commandMessage "\[$commandID] $commandName pos = $pos"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cf $commandID $pos]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


iwidgets::regexpfield $tab.fLinearDelta.pos \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "deltapos (mm):"\
    -textvariable "${ns}::pos" \
    -width 8


button $tab.fLinearDelta.b$suffix \
    -text "Delta Pos" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fLinearStage1.b$suffix \
#    -side right
#pack $tab.fLinearStage1.pos\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fLinearDelta\
	-in $tab\
	-row 5\
	-column 0

grid $tab.fLinearDelta.b$suffix \
    -in $tab.fLinearDelta\
    -row 1\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fLinearDelta.pos\
    -in $tab.fLinearDelta\
    -row 1\
    -column 0\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLinearDelta.b$suffix $tthelp


}

