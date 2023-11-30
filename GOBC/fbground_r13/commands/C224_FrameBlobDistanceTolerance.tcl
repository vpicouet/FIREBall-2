set ns "C224"
namespace eval $ns {
variable commandID 224


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $blobTab
set suffix FBDistTol


# -----------------------------
# Help Variables
# -----------------------------
set tthelp\
"Command ID $commandID
-----------------------------------
Sets the Distance Tolerance parameter 
for the frameblob procedures. See 
blobs.h and FrameBlob.h."


variable fb -50.0
variable fbval $fb


# -----------------------------
# Namespace Variables
# -----------------------------

variable commandName "FrameBlob Dist Tol"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Command ID $commandID
-----------------------
Do you want to set the
FrameBlob Distance Tolerance to:
   DistTol = $fb"

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
    if { $fb < 1 } { set fb 1 }
    if { $fb > 2000 } { set fb 200 }


variable commandQuestion \
"Command ID $commandID
---------------------------
Do you want to set the
FrameBlob Distance Tolerance to:

   DistTol = $fb"

    set commandMessage "\[$commandID] $commandName DistTol= $fb"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cs $commandID $fb ]
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
    -text "FrameBlob Distance Tolerance"\


grid $tab.f$suffix -in $tab -column 1 -row 2 -sticky ew


iwidgets::regexpfield $tab.f$suffix.ref$suffix \
    -regexp {^[0-9]+$}\
    -labeltext "DistTol:"\
    -textvariable "${ns}::fb" \
    -textfont {Helvetica 14 bold} \
    -width 5

button $tab.f$suffix.b$suffix \
    -text "Send" \
    -font {Helvetica 16 bold} \
    -command "${ns}::myfunction"

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

