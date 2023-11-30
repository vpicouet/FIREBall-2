set ns "C117"
namespace eval $ns {
variable commandID 117


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tStackSolve

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Stack and solve the field.
"


variable stacksize 10


# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Stack and Solve"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to stack and solve $stacksize images?"


# -----------------------------
# Function
# -----------------------------
proc myfunction { solv } {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable stacksize
    variable stack 1
    variable solve 0
    
    if { $stacksize < 1 } { set stacksize 1 }
    if { $stacksize > 100 } { set stacksize 100 }

    if { $solv < 0 } { set solv 0 }
    if { $solv > 2 } { set solv 2 }
    set solve $solv

set commandQuestion \
"Do you want to stack and solve $stacksize images?"

    set commandMessage "\[$commandID] $commandName stack=$stacksize solve=$solve"
    set commandLog $commandMessage
    if { [verify "$commandQuestion"]} {
	set mbody [binary format ccusucu $commandID $stack $stacksize $solve ]
	message $mbody $commandMessage $commandLog
    } else {

    }
}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fStackSolve \
    -borderwidth 2 \
    -relief ridge \
    -text "Stack and Solve"

pack $tab.fStackSolve

iwidgets::regexpfield $tab.fStackSolve.stacksize \
    -regexp {^[0-9]+$}\
    -labeltext "stack size:"\
    -textvariable "${ns}::stacksize" \
    -width 8

button $tab.fStackSolve.bStackSolveNone \
    -text "No Solt'n" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 0"\
    -width 10

button $tab.fStackSolve.bStackSolveProx \
    -text "Prox." \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 1"\
    -width 10

button $tab.fStackSolve.bStackSolveLost \
    -text "Lost" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction 2"\
    -width 10

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fStackSolve.stacksize\
    -in $tab.fStackSolve\
    -row 0\
    -column 0\
    -columnspan 1

grid $tab.fStackSolve.bStackSolveNone\
    -in $tab.fStackSolve\
    -row 0\
    -column 1

grid $tab.fStackSolve.bStackSolveProx\
    -in $tab.fStackSolve\
    -row 0\
    -column 2

grid $tab.fStackSolve.bStackSolveLost\
    -in $tab.fStackSolve\
    -row 0\
    -column 3


# ----------------------------
# Tooltip Implementation
# ----------------------------
tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

