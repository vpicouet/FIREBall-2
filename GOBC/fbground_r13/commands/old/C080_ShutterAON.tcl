set ns "C080"
namespace eval $ns {
variable commandID 80
variable shutterName "Shutter A"

# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P080Tab

# -----------------------------
# frame for shutter A 
# -----------------------------
ttk::labelframe $tab.shutterA \
    -borderwidth 2 \
    -relief ridge \
    -text $shutterName




# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Opens $shutterName"


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Open $shutterName"
variable commandQuestion "Do you want to\n open $shutterName?"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    if { [verify "$commandQuestion"]} {
	set mbody [binary format c $commandID]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

if { $positions::P080On } {
grid  $tab.shutterA\
    -in $tab\
    -row $positions::P080Y\
    -column $positions::P080X\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

button $tab.shutterA.shutterAON \
    -text "ON" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"

pack $tab.shutterA.shutterAON \
    -side left\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.shutterA.shutterAON $tthelp


}

