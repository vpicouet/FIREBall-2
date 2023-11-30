set ns "C081"
namespace eval $ns {
variable commandID 81


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::P081Tab


# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"COMMAND ID $commandID
------------------------------
Opens shutter A"


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Close shutter A"
variable commandQuestion "Do you want to\n close shutter A?"
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

#ttk::labelframe $tab.shutterA \
#    -borderwidth 2 \
#    -relief ridge \
#    -text "Shutter A"

if { $positions::P081On } {
grid  $tab.shutterA\
    -in $tab\
    -row $positions::P081Y\
    -column $positions::P081X\
    -sticky ew\
    -padx $positions::padX\
    -pady $positions::padY
}

button $tab.shutterA.shutterAOFF \
    -text "OFF" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"

pack $tab.shutterA.shutterAOFF \
    -side left\
    -fill x



# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.shutterA.shutterAOFF $tthelp


}

