set ns "C094"
namespace eval $ns {
variable commandID 94


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tDither
set suffix Dither

ttk::labelframe $tab.fDither \
    -borderwidth 2 \
    -relief ridge \
    -text "Dither"

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Load and execute dither pattern
"


variable pos 4
variable del 5
variable det 0


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Dither"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to execute dither
pattern $pos with delay $del ?"


# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable pos
    variable del
    variable det

    if { $pos < 1} { set pos 1 }
    if { $pos > 255 } { set pos 255 }

    if { [expr ($del < 5) ] } { set del 5 }
    if { [expr ($del > 127.0) ] } { set del 127 }
    set delay $del
    if { $det > 0 } { set delay [expr $del * (-1) ] }
    set det 0
    

set commandQuestion \
"Do you want to execute dither pattern $pos
with delay $del? "

    set commandMessage "\[$commandID] $commandName pos = $pos"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccc $commandID [expr $delay & 0xff] [expr $pos & 0xff] ]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


iwidgets::regexpfield $tab.fDither.pos \
    -regexp {^[0-9]+$}\
    -labeltext "Pattern:"\
    -textvariable "${ns}::pos" \
    -width 8

iwidgets::regexpfield $tab.fDither.del \
    -regexp {^[\-0-9]+$}\
    -labeltext "Delay (s):"\
    -textvariable "${ns}::del" \
    -width 8


button $tab.fDither.b$suffix \
    -text "Dither!" \
    -font {Helvetica 10 bold} \
    -command "${ns}::myfunction"\
    -width 15

ttk::checkbutton $tab.fDither.det \
    -variable "${ns}::det" \
    -offvalue 0 \
    -onvalue 1 \
    -text "Det"

#pack $tab.fLinearStage1.b$suffix \
#    -side right
#pack $tab.fLinearStage1.pos\
#    -fill x \
#    -side left\
#    -expand 1

pack $tab.fDither
#    -in $tab\
#    -row 5\
#    -column 0

grid $tab.fDither.b$suffix \
    -in $tab.fDither\
    -row 1\
    -column 4\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fDither.pos\
    -in $tab.fDither\
    -row 1\
    -column 0\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fDither.del\
    -in $tab.fDither\
    -row 1\
    -column 2\
    -columnspan 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $tab.fDither.det\
    -in $tab.fDither\
    -row 1\
    -column 5
# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fDither.b$suffix $tthelp


}

