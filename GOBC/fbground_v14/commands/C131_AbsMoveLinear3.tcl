set ns "C131"
namespace eval $ns {
variable commandID 131


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::LinearStageTab3
set suffix LinearPos

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Command an absolute
linear stage position in
mm
"


variable pos 0.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Linear stage 3 absolute"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to send position:
pos = $pos mm
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
stage pos:
pos = $pos mm
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


iwidgets::regexpfield $tab.fLinearStage3.pos \
    -regexp {^[\-\.0-9]+$}\
    -labeltext "abspos (mm):"\
    -textvariable "${ns}::pos" \
    -width 8


button $tab.fLinearStage3.b$suffix \
    -text "Abs. Pos C" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fLinearStage3.b$suffix \
#    -side right
#pack $tab.fLinearStage3.pos\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fLinearStage3.b$suffix \
    -in $tab.fLinearStage3\
    -row 1\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fLinearStage3.pos\
    -in $tab.fLinearStage3\
    -row 1\
    -column 0\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fLinearStage3.b$suffix $tthelp


}

