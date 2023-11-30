set ns "C223"
namespace eval $ns {
variable commandID 223


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBThreshhold

# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
--------------------------------
Sets the Threshhold parameter for the 
frameblob procedures. See 
blobs.h and FrameBlob.h."


variable fb -5.0
variable fbval $fb


# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Threshhold"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Threshhold to:
   Thr = $fb"

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
    if { $fb > 65535 } { set fb 65535 }


variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Threshhold to:
   Thr = $fb"

    set commandMessage "\[$commandID] $commandName Threshhold= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cf $commandID $fb ]
	message $mbody $commandMessage $commandLog
    } else {
#	.stars create oval 100 200 300 200 -outline green -fill red
    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------

ttk::labelframe $tab.f$suffix \
    -relief ridge \
    -borderwidth 2 \
    -text "FrameBlob Threshhold"\


grid $tab.f$suffix -in $tab -column 2 -row 2 -sticky ew


iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[\.0-9]+$}\
    -labeltext "Thr:"\
    -textvariable "${ns}::fb" \
    -textfont {Helvetica 14 bold} \
    -width 10

button $tab.f$suffix.b$suffix \
    -text "Send" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"\

ttk::label $tab.f$suffix.l$suffix \
    -textvariable "${ns}::fbval" \
    -font {Helvetica 14 bold} \
    -foreground blue \
    -width 10 

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

