set ns "C043"
namespace eval $ns {
variable commandID 43


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tChangeSubtract

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Change the algorithm used for centroiding
0 -- barycenter 1
1 -- framblobs
2 -- ???
"


variable target 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change centroiding algorithm"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change centroiding algorithm
0 -- barycenter 1
1 -- frameblobs -- discouraged
2 -- Gaussian
Centroiding = $target\n
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
    variable target

    puts " ---> $target "
    if { [expr ( $target < 0 ) ] } { set target 0 }
    if { [expr ( $target > 2 ) ] } { set target 2 }
    puts " ------> $target"
    set target [expr { $target & 0xFF } ]

set commandQuestion \
"Do you want to change subtraction to $target \n
0=barycenter, 1=frameblob, 2=Gaussian, 3+=Surprise!
"   

    set commandMessage "\[$commandID] $commandName target=$target"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID [expr {0xFF & $target}]]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fChangeAlgorithm \
    -borderwidth 2 \
    -relief ridge \
    -text "Centroiding Algorithm"

pack $tab.fChangeAlgorithm

iwidgets::regexpfield $tab.fChangeAlgorithm.target \
    -regexp {^[0-9]+$}\
    -labeltext "Algorithm:"\
    -textvariable "${ns}::target" \
    -width 8


button $tab.fChangeAlgorithm.bChangeAlgorithm \
    -text "Change Algorithm" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15


ttk::label $tab.fChangeAlgorithm.value \
    -textvariable "var::algorithm" \
    -width 8 \
    -justify right\
    -foreground blue


#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fChangeAlgorithm.bChangeAlgorithm\
    -in $tab.fChangeAlgorithm\
    -row 2\
    -column 2

grid $tab.fChangeAlgorithm.target\
    -in $tab.fChangeAlgorithm\
    -row 2\
    -column 0\
    -columnspan 2


grid $tab.fChangeAlgorithm.value\
    -in $tab.fChangeAlgorithm\
    -row 2\
    -column 5\
    -columnspan 2

# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

