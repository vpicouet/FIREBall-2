set ns "C047"
namespace eval $ns {
variable commandID 47


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tNewTarget

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Send new targets to guider
"


variable x0 0
variable y0 0
variable x1 0
variable y1 0
variable x2 0
variable y2 0
variable x3 0
variable y3 0
variable x4 0
variable y4 0
variable x5 0
variable y5 0
variable x6 0
variable y6 0
variable x7 0
variable y7 0
variable u0 0
variable u1 0
variable u2 0
variable u3 0
variable u4 0
variable u5 0
variable u6 0
variable u7 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Send new targets to guider"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to send new targets to guider? 
x = $x0, $x1, $x2, $x3, $x4, $x5, $x6, $x7
y = $y0, $y1, $y2, $y3, $y4, $y5, $y6, $y7
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
    variable x0
    variable y0
    variable x1
    variable y1
    variable x2
    variable y2
    variable x3
    variable y3
    variable x4
    variable y4
    variable x5
    variable y5
    variable x6
    variable y6
    variable x7
    variable y7
    variable u0
    variable u1
    variable u2
    variable u3
    variable u4
    variable u5
    variable u6
    variable u7



set commandQuestion \
"
Do you want to send new targets to guider?
x = $x0, $x1, $x2, $x3, $x4, $x5, $x6, $x7
y = $y0, $y1, $y2, $y3, $y4, $y5, $y6, $y7
"   
    set use [ expr {$u0+$u1+$u2+$u3+$u4+$u5+$u6+$u7} ]
    puts "here..."

    if {  $use > 0.0 } { 


	for {set i 0} { $i < 8 } { incr i } {
	    puts u$i
	    if { [set u$i] == 0 } { 
		set x$i -100
		set y$i -100
	    }
	}

	set commandMessage "\[$commandID] $commandName x = $x0, $x1, $x2, $x3, $x4, $x5, $x6, $x7; y = $y0, $y1, $y2, $y3, $y4, $y5, $y6, $y7"

    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cssssssssssssssss $commandID $x0 $x1 $x2 $x3 $x4 $x5 $x6 $x7 $y0 $y1 $y2 $y3 $y4 $y5 $y6 $y7]
	message $mbody $commandMessage $commandLog
    } else {

    }
} else {
   tk_messageBox -message "No target points selected" -type ok
}
}
puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fNewTarget \
    -borderwidth 2 \
    -relief ridge \
    -text "New Target"

pack $tab.fNewTarget


button $tab.fNewTarget.bNewTarget \
    -text "New Targets" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 20

 for {set i 0} { $i < 8 } { incr i } { 

iwidgets::regexpfield $tab.fNewTarget.x$i \
    -labeltext "x$i:"\
    -textvariable "${ns}::x$i" \
    -width 5

iwidgets::regexpfield $tab.fNewTarget.y$i \
    -labeltext "y$i:"\
    -textvariable "${ns}::y$i" \
    -width 5

checkbutton $tab.fNewTarget.cNewTarget$i \
    -variable "${ns}::u$i" \
    -onvalue 1 \
    -offvalue 0

grid $tab.fNewTarget.x$i\
    -in $tab.fNewTarget\
    -row $i \
    -column 0

grid $tab.fNewTarget.y$i\
    -in $tab.fNewTarget\
    -row $i \
    -column 1

grid $tab.fNewTarget.cNewTarget$i \
    -in $tab.fNewTarget \
    -row $i \
    -column 2

}

grid $tab.fNewTarget.bNewTarget\
    -in $tab.fNewTarget\
    -row 8\
    -column 0\
    -columnspan 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
# tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

