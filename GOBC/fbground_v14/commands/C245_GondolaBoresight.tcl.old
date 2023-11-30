set ns "C245"
namespace eval $ns {
variable commandID 245


# -----------------------------
# pathname for the controls
# -----------------------------
set tab $positions::tGondolaBoresight

variable x0 0
variable y0 0
variable x1 0 
variable y1 0 
variable dEL 0 
variable dCE 0 

# -----------------------------
# Function
# -----------------------------
proc myfunction {} {
    variable x0
    variable x1
    variable y0
    variable y1
    variable dEL 
    variable dCE
    set el $var::gel
    set az $var::gaz
    set rot $var::grot

    set platescale 0.9
    set dx [expr $x1-$x0]
    set dy [expr $y1-$y0]

    set srot [expr sin($rot / 180.0 * 3.14159) ]
    set crot [expr cos($rot / 180.0 * 3.14159) ]

    set dEL [ format {%8.3f} [expr ($dx*$crot - $dy*$srot)*$platescale]]
    set dCE [ format {%8.3f} [expr ($dx*$srot + $dy*$crot)*$platescale]]

#direction_el = direction_ce = 1
# image 0,0 is in the top corner
#    pts[0].x = SHAPE_SIZE*(DIRECTION_CE)*crot;
#    pts[0].y = SHAPE_SIZE*(DIRECTION_CE)*srot;
#    pts[1].x = 0;
#    pts[1].y = 0;
#    pts[2].x = -2*SHAPE_SIZE*(DIRECTION_EL)*srot;
#    pts[2].y = 2*SHAPE_SIZE*(DIRECTION_EL)*crot;


}


puts $ns

# ----------------------------
# Required Widgets
# ----------------------------


ttk::labelframe $tab.fGondolaBoresight \
    -borderwidth 2 \
    -relief ridge \
    -text "Gondola Boresight and Correction"

pack $tab.fGondolaBoresight

ttk::label $tab.fGondolaBoresight.azimuth\
    -text "Azimuth:"\
    -font {Helvetica 14 bold} 

ttk::label $tab.fGondolaBoresight.azimuth_value\
    -textvariable "var::gaz"\
    -font {Helvetica 14 bold} \
    -foreground blue\
    -justify center

ttk::label $tab.fGondolaBoresight.elevation\
    -text "Elevation:"\
    -font {Helvetica 14 bold} 

ttk::label $tab.fGondolaBoresight.elevation_value\
    -textvariable "var::gel"\
    -font {Helvetica 14 bold} \
    -foreground blue\
    -justify center

ttk::label $tab.fGondolaBoresight.rotation\
    -text "Rotation:"\
    -font {Helvetica 14 bold} 

ttk::label $tab.fGondolaBoresight.rotation_value\
    -textvariable "var::grot"\
    -font {Helvetica 14 bold} \
    -foreground blue\
    -anchor center

ttk::label $tab.fGondolaBoresight.startlabel\
    -text "Initial (x,y)"\
    -font {Helvetica 14 bold} \
    -foreground black\
    -anchor center

ttk::label $tab.fGondolaBoresight.endlabel\
    -text "  Final (x,y)"\
    -font {Helvetica 14 bold} \
    -foreground black\
    -anchor center

iwidgets::regexpfield $tab.fGondolaBoresight.x0 \
    -labeltext "xi:"\
    -textvariable "${ns}::x0" \
    -fixed 14\
    -regexp {^[-0-9]+$}\
    -width 6

iwidgets::regexpfield $tab.fGondolaBoresight.y0 \
    -labeltext "yi:"\
    -textvariable "${ns}::y0" \
    -fixed 16\
    -regexp {^[-0-9]+$}\
    -width 6

iwidgets::regexpfield $tab.fGondolaBoresight.x1 \
    -labeltext "xf:"\
    -textvariable "${ns}::x1" \
    -fixed 14\
    -regexp {^[-0-9]+$}\
    -width 6

iwidgets::regexpfield $tab.fGondolaBoresight.y1 \
    -labeltext "yf:"\
    -textvariable "${ns}::y1" \
    -fixed 16\
    -regexp {^[-0-9]+$}\
    -width 6


ttk::label $tab.fGondolaBoresight.dEL\
    -text "dEL:"\
    -font {Helvetica 14 bold} 

ttk::label $tab.fGondolaBoresight.dEL_value\
    -textvariable "${ns}::dEL"\
    -font {Helvetica 14 bold} \
    -foreground red\
    -justify center

ttk::label $tab.fGondolaBoresight.dCE\
    -text "dCE:"\
    -font {Helvetica 14 bold} 

ttk::label $tab.fGondolaBoresight.dCE_value\
    -textvariable "${ns}::dCE"\
    -font {Helvetica 14 bold} \
    -foreground red\
    -justify center




grid $tab.fGondolaBoresight.azimuth\
    -in $tab.fGondolaBoresight\
    -row 1\
    -column 1

grid $tab.fGondolaBoresight.azimuth_value\
    -in $tab.fGondolaBoresight\
    -row 1\
    -column 2

grid $tab.fGondolaBoresight.elevation\
    -in $tab.fGondolaBoresight\
    -row 2\
    -column 1

grid $tab.fGondolaBoresight.elevation_value\
    -in $tab.fGondolaBoresight\
    -row 2\
    -column 2

grid $tab.fGondolaBoresight.rotation\
    -in $tab.fGondolaBoresight\
    -row 3\
    -column 1

grid $tab.fGondolaBoresight.rotation_value\
    -in $tab.fGondolaBoresight\
    -row 3\
    -column 2

grid $tab.fGondolaBoresight.startlabel\
    -in $tab.fGondolaBoresight\
    -row 4\
    -column 1

grid $tab.fGondolaBoresight.endlabel\
    -in $tab.fGondolaBoresight\
    -row 5\
    -column 1

grid $tab.fGondolaBoresight.x0 \
    -in $tab.fGondolaBoresight\
    -row 4\
    -column 2

grid $tab.fGondolaBoresight.y0 \
    -in $tab.fGondolaBoresight\
    -row 4\
    -column 3

grid $tab.fGondolaBoresight.x1 \
    -in $tab.fGondolaBoresight\
    -row 5\
    -column 2

grid $tab.fGondolaBoresight.y1 \
    -in $tab.fGondolaBoresight\
    -row 5\
    -column 3

grid $tab.fGondolaBoresight.dEL\
    -in $tab.fGondolaBoresight\
    -row 6\
    -column 2

grid $tab.fGondolaBoresight.dEL_value\
    -in $tab.fGondolaBoresight\
    -row 6\
    -column 3

grid $tab.fGondolaBoresight.dCE\
    -in $tab.fGondolaBoresight\
    -row 7\
    -column 2

grid $tab.fGondolaBoresight.dCE_value\
    -in $tab.fGondolaBoresight\
    -row 7\
    -column 3


button $tab.fGondolaBoresight.bCalculate\
    -text "Compute"\
    -font {Helvetica 14 bold} \
    -command "${ns}::myfunction"\
    -width 15

grid $tab.fGondolaBoresight.bCalculate\
    -in $tab.fGondolaBoresight\
    -row 6\
    -column 1\
    -rowspan 2


}

