set ns "C229"
namespace eval $ns {
variable commandID 229


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBReadoutNoise

# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
--------------------------------
Sets the Readout Noise for the 
frameblob procedures. See 
blobs.h and FrameBlob.h."


variable fb -100
variable fbval $fb

# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Readout Noise"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Readout Noise to:
   RN = $fb"

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable fb

# some boundary checking
    if { $fb < 0.001 } { set fb 0.001 }
    if { $fb > 1000 } { set fb 1000 }


variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Readout Noise to:
   RN = $fb"

    set commandMessage "\[$commandID] $commandName RN= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cf $commandID [ expr $fb * 1.0 ] ]
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
    -text "FrameBlob Readout Noise"\


grid $tab.f$suffix -in $tab -column 2 -row 5 -sticky ew


iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[\.0-9]+$}\
    -labeltext "RN:"\
    -textvariable "${ns}::fb" \
    -textfont {Helvetica 14 bold} \
    -width 10

ttk::label $tab.f$suffix.l$suffix \
    -textvariable "${ns}::fbval" \
    -font {Helvetica 14 bold} \
    -foreground blue \
    -width 10 

button $tab.f$suffix.b$suffix \
    -text "Send" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

pack $tab.f$suffix.b$suffix \
    -side right


pack $tab.f$suffix.ref$suffix\
    -fill x \
    -side left\
    -side top\
    -expand 1

pack $tab.f$suffix.l$suffix \
    -fill y\
    -side bottom\
    -expand 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.f$suffix.b$suffix $tthelp


}

