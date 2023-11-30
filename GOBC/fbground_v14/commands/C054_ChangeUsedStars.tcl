set ns "C054"
namespace eval $ns {
variable commandID 54


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tChangeUsed

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Set the Guider Mode.
"

variable usemask 0
variable ustar0 1
variable ustar1 2
variable ustar2 4
variable ustar3 0
variable ustar4 0
variable ustar5 0
variable ustar6 0
variable ustar7 0




# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Change Used Stars"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"
Do you want to send the use mask:
$usemask?
"


    proc cbox0 { } {

    }

# -----------------------------
# Function
# -----------------------------
proc myfunction {  } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable ustar0
    variable ustar1
    variable ustar2
    variable ustar3
    variable ustar4
    variable ustar5
    variable ustar6
    variable ustar7
    variable tab

    set starmask [expr {$ustar0 + $ustar1 + $ustar2 + $ustar3 + $ustar4 +\
			    $ustar5 + $ustar6 + $ustar7 }]
    if { $starmask < 0 } { set starmask 0  }
    if { $starmask > 255 } { set starmask 255  }

    puts "$starmask $ustar0 $ustar1"

    set commandQuestion \
    "
    Do you want to set star use mask to
    $starmask
    "   

    set commandMessage "\[$commandID] $commandName $starmask"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format cc $commandID $starmask]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fChangeUsed \
    -borderwidth 2 \
    -relief ridge \
    -text "Change Used Stars"

pack $tab.fChangeUsed

button $tab.fChangeUsed.bChangeUsed \
    -text "Change Used" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

ttk::checkbutton $tab.fChangeUsed.cStar0 \
    -offvalue 0 \
    -onvalue 1 \
    -variable "${ns}::ustar0" \
    -text "S0"

ttk::checkbutton $tab.fChangeUsed.cStar1 \
    -variable "${ns}::ustar1" \
    -offvalue 0 \
    -onvalue 2 \
    -text "S1"

ttk::checkbutton $tab.fChangeUsed.cStar2 \
    -variable "${ns}::ustar2" \
    -offvalue 0 \
    -onvalue 4 \
    -text "S2"

ttk::checkbutton $tab.fChangeUsed.cStar3 \
    -variable "${ns}::ustar3" \
    -offvalue 0 \
    -onvalue 8 \
    -text "S3"

ttk::checkbutton $tab.fChangeUsed.cStar4 \
    -variable "${ns}::ustar4" \
    -offvalue 0 \
    -onvalue 16 \
    -text "S4"

ttk::checkbutton $tab.fChangeUsed.cStar5 \
    -variable "${ns}::ustar5" \
    -offvalue 0 \
    -onvalue 32 \
    -text "S5"

ttk::checkbutton $tab.fChangeUsed.cStar6 \
    -variable "${ns}::ustar6" \
    -offvalue 0 \
    -onvalue 64 \
    -text "S6"

ttk::checkbutton $tab.fChangeUsed.cStar7 \
    -variable "${ns}::ustar7" \
    -offvalue 0 \
    -onvalue 128 \
    -text "S7"


grid $tab.fChangeUsed.cStar0\
    -in $tab.fChangeUsed\
    -row 0\
    -column 0

grid $tab.fChangeUsed.cStar1\
    -in $tab.fChangeUsed\
    -row 0\
    -column 1

grid $tab.fChangeUsed.cStar2\
    -in $tab.fChangeUsed\
    -row 0\
    -column 2

grid $tab.fChangeUsed.cStar3\
    -in $tab.fChangeUsed\
    -row 0\
    -column 3

grid $tab.fChangeUsed.cStar4\
    -in $tab.fChangeUsed\
    -row 1\
    -column 0

grid $tab.fChangeUsed.cStar5\
    -in $tab.fChangeUsed\
    -row 1\
    -column 1

grid $tab.fChangeUsed.cStar6\
    -in $tab.fChangeUsed\
    -row 1\
    -column 2

grid $tab.fChangeUsed.cStar7\
    -in $tab.fChangeUsed\
    -row 1\
    -column 3

grid $tab.fChangeUsed.bChangeUsed\
    -in $tab.fChangeUsed\
    -row 0\
    -column 5

}

