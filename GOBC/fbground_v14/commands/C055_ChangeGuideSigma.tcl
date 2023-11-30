set ns "C055"
namespace eval $ns {
variable commandID 55


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tChangeGuideSigma

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Change the guide star sigma threshholds
"


variable x 1.0
variable y 15.0
variable z 5.0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change guide star sigmas"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to change guide star sigmas to
thresh = $z
sigmin = $x
sigmax = $y 
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
    variable x
    variable y
    variable z




    if { $z < 0.3 } { set z 0.3 } 
    if { $z > 15.8 } { set z 15.8 } 

    if { $x < 0.1 } { set x 0.1 } 
    if { $x > 31.999 } { set x 31.999 }
    
    if { $y < 0.1 } { set y 0.1 }
    if { $y > 31.999 } { set y 31.999 } 

    set commandQuestion \
"
Do you want to change guide star sigma to
thresh = $z
sigmin = $x
sigmax = $y 
"   

    set uthresh [expr {round( $z * 16. ) }]
    set usigmin [expr {round( $x * 2048.) }]
    set usigmax [expr {round( $y * 2048.) }]
    puts " $x $y $usigmin $usigmax "

    set commandMessage "\[$commandID] $commandName sigmin=$x sigmax=$y"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccss $commandID $uthresh $usigmin $usigmax]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fChangeSigma \
    -borderwidth 2 \
    -relief ridge \
    -text "Change Guide Star Sigma"

pack $tab.fChangeSigma 

iwidgets::regexpfield $tab.fChangeSigma.z \
    -labeltext "snr:"\
    -textvariable "${ns}::z" \
    -width 6

ttk::label $tab.fChangeSigma.lz \
    -textvariable "var::thresh" \
    -width 6 \
    -anchor center\
    -relief sunken\
    -foreground blue 


iwidgets::regexpfield $tab.fChangeSigma.x \
    -labeltext "min (pix):"\
    -textvariable "${ns}::x" \
    -width 6

ttk::label $tab.fChangeSigma.lx \
    -textvariable "var::smin" \
    -width 6 \
    -anchor center\
    -relief sunken\
    -foreground blue 



iwidgets::regexpfield $tab.fChangeSigma.y \
    -labeltext "max (pix):"\
    -textvariable "${ns}::y" \
    -width 6

ttk::label $tab.fChangeSigma.ly \
    -textvariable "var::smax" \
    -width 6 \
    -anchor center\
    -relief sunken\
    -foreground blue 

button $tab.fChangeSigma.bChangeSigma \
    -text "Thr+Sigma" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 14

grid $tab.fChangeSigma.z\
    -in $tab.fChangeSigma\
    -row 0 \
    -column 0

grid $tab.fChangeSigma.lz\
    -in $tab.fChangeSigma\
    -row 1\
    -column 0

grid $tab.fChangeSigma.x\
    -in $tab.fChangeSigma\
    -row 0 \
    -column 1

grid $tab.fChangeSigma.lx\
    -in $tab.fChangeSigma\
    -row 1\
    -column 1


grid $tab.fChangeSigma.y\
    -in $tab.fChangeSigma\
    -row 0 \
    -column 2

grid $tab.fChangeSigma.ly\
    -in $tab.fChangeSigma\
    -row 1\
    -column 2

grid $tab.fChangeSigma.bChangeSigma\
    -in $tab.fChangeSigma\
    -row 0\
    -column 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

