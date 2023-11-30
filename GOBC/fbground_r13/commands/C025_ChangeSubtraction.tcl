set ns "C025"
namespace eval $ns {
variable commandID 25


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
Change what is subtracted from the images
0 -- nothing
1 -- saved median
2 -- row noise
3 -- both
"


variable target 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change subtracted items"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to change subtraction to
0= none, 1= median, 2=rows, 3=both?
subtract = $target\n
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

    if { $target < 0 } { set target 0 }
    if { $target > 3 } { set target 3 }

    set target [expr { $target & 0xFF } ]

set commandQuestion \
"Do you want to change subtraction to
0=none, 1=median, 2=rows, 3=both
target = $target\n
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


ttk::labelframe $tab.fChangeSubtract \
    -borderwidth 2 \
    -relief ridge \
    -text "Image Subtract"

pack $tab.fChangeSubtract

iwidgets::regexpfield $tab.fChangeSubtract.target \
    -regexp {^[0-9]+$}\
    -labeltext "Sub Mask:"\
    -textvariable "${ns}::target" \
    -width 8

ttk::label $tab.fChangeSubtract.value \
    -textvariable "var::subtract" \
    -width 8 \
    -justify right\
    -foreground blue


button $tab.fChangeSubtract.bChangeSubtract \
    -text "Change Sub Mask" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fChangeSubtract.bChangeSubtract\
    -in $tab.fChangeSubtract\
    -row 2\
    -column 2

grid $tab.fChangeSubtract.target\
    -in $tab.fChangeSubtract\
    -row 2\
    -column 0\
    -columnspan 2

grid $tab.fChangeSubtract.value\
    -in $tab.fChangeSubtract\
    -row 2\
    -column 5\
    -columnspan 2
# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

