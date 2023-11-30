set ns "C230"
namespace eval $ns {
variable commandID 230


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBReset

# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
--------------------------------
Resets frameblob parameters to defaults
blobs.h and FrameBlob.h."



# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Reset"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to reset
frameblob parameters?"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog

# some boundary checking

variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to reset
frameblob parameters"

    set commandMessage "\[$commandID] $commandName"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format c $commandID ]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -text "FrameBlob Parameter Reset"


grid $tab.f$suffix -in $tab -column 1 -row 5 -sticky ew



button $tab.f$suffix.b$suffix \
    -text "Reset" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.f$suffix.b$suffix 


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

