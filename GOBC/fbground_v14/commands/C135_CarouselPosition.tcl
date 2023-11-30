set ns "C135"
namespace eval $ns {
variable commandID 135


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tCarouselPosition

# -----------------------------
# Help Variables
# -----------------------------
set tthelp \
"Command ID $commandID
---------------------------------
Move to a particular carousel position
"


    variable carousel 0
    variable poslist { "New QSO (-141)" "Old QSO (-101.25)" "Field 1 (-61.1)" " Field 4 (-21)" "Field 2 (19)" "Field 3 (58.5)" "Grid (99)" "Open (139)" "Tilted (179)" }

# -----------------------------
# Namespace Variables
# -----------------------------
variable commandName "Load carousel position"
variable commandMessage "\[$commandID] $commandName"
variable commandLog $commandMessage
variable commandQuestion \
"Do you want to load carousel position:
carousel = $carousel\n
"

variable mytab $tab
# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable commandID
    variable commandName
    variable commandQuestion
    variable commandMessage
    variable commandLog
    variable poslist
    variable mytab

    set carouselnow [ $mytab.fCarouselPosition.sCarouselPosition get ]
    set carousel [ $mytab.fCarouselPosition.sCarouselPosition current ]
    puts $carouselnow
    

    set commandQuestion \
"Do you want to load carousel position:
carousel = $carouselnow : $carousel
"

    puts "$carouselnow $carousel"

    
    set commandMessage "\[$commandID] $commandName carousel=$carouselnow"
    set commandLog $commandMessage
    if { [verify "$commandQuestion" ] } {
	set mbody [ binary format cc $commandID [ expr { 0xFF & $carousel } ] ]
	message $mbody $commandMessage $commandLog
    } else {
	
    }
}



puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fCarouselPosition \
    -borderwidth 2 \
    -relief ridge \
    -text "Carousel Position"



if { $positions::P135On } {
    grid $tab.fCarouselPosition\
	-in $tab\
	-row $positions::CarouselPositionY\
	-column $positions::CarouselPositionX\
	-padx $positions::padX\
	-pady $positions::padY
}


#iwidgets::regexpfield $tab.fLoadTarget.target \
#    -regexp {^[0-9]+$}\
#    -labeltext "Target ID:"\
#    -textvariable "${ns}::target" \
#    -width 8



ttk::combobox $tab.fCarouselPosition.sCarouselPosition \
    -width 16 \
    -values "$poslist"\
    -font {Helvetica 14 bold}



$tab.fCarouselPosition.sCarouselPosition set [ lindex $poslist 0]


button $tab.fCarouselPosition.bCarouselPosition \
    -text "Position" \
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

#pack $tab.fRotaryStage.b$suffix \
#    -side right
#pack $tab.fRotaryStage.angle\
#    -fill x \
#    -side left\
#    -expand 1

grid $tab.fCarouselPosition.bCarouselPosition\
    -in $tab.fCarouselPosition\
    -row 0\
    -column 2

grid $tab.fCarouselPosition.sCarouselPosition\
    -in $tab.fCarouselPosition\
    -row 0\
    -column 1


# ----------------------------
# Tooltip Implementation
# ----------------------------
#tooltip::tooltip $tab.fSetExpTime.bExpTime $tthelp


}

