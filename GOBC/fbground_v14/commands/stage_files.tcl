source "$MAINPATH/commands/C100_HomeRotary.tcl"
source "$MAINPATH/commands/C101_AbsMoveRotary.tcl"

ttk::label $stageTab.fRotaryStage.tRotaryStage\
    -text "Angle (deg):"\
    -anchor center\


ttk::label $stageTab.fRotaryStage.tRotaryStageAngle\
    -textvariable var::stagerpos\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fRotaryStage.fRotaryStageErr\
    -textvariable var::stagererr\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fRotaryStage.fRotaryPosErr\
    -textvariable var::posrerr\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fRotaryStage.fRotaryStageState\
    -textvariable var::stagerstat\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

grid $stageTab.fRotaryStage.tRotaryStage\
    -in $stageTab.fRotaryStage\
    -row 0\
    -column 1\
    -padx $positions::padX\
    -pady $positions::padY

grid $stageTab.fRotaryStage.tRotaryStageAngle\
    -in $stageTab.fRotaryStage\
    -row 0\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

grid $stageTab.fRotaryStage.fRotaryStageErr\
    -in $stageTab.fRotaryStage\
    -row 3\
    -column 0

grid $stageTab.fRotaryStage.fRotaryPosErr\
    -in $stageTab.fRotaryStage\
    -row 3\
    -column 1

grid $stageTab.fRotaryStage.fRotaryStageState\
    -in $stageTab.fRotaryStage\
    -row 3\
    -column 2
    

#pack $stageTab.fRotaryStage.tRotaryStage $stageTab.fRotaryStage.tRotaryStageAngle


source "$MAINPATH/commands/C110_HomeLinear1.tcl"
source "$MAINPATH/commands/C111_AbsMoveLinear1.tcl"


ttk::label $stageTab.fLinearStage1.tLinearStage\
    -text "Position (mm):"\
    -anchor center\


ttk::label $stageTab.fLinearStage1.tLinearStageAngle\
    -textvariable var::stage1pos\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage1.fLinear1StageErr\
    -textvariable var::stage1err\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage1.fLinear1PosErr\
    -textvariable var::pos1err\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage1.fLinear1StageState\
    -textvariable var::stage1stat\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center




grid $stageTab.fLinearStage1.tLinearStage\
    -in $stageTab.fLinearStage1\
    -row 0\
    -column 1\
    -padx $positions::padX\
    -pady $positions::padY

grid $stageTab.fLinearStage1.tLinearStageAngle\
    -in $stageTab.fLinearStage1\
    -row 0\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY

#grid $stageTab.fLinearStage1.tLinear1StageAngle\
#    -in $stageTab.fLinearStage1\
#    -row 0\
#    -column 2\
#    -padx $positions::padX\
#    -pady $positions::padY

grid $stageTab.fLinearStage1.fLinear1StageErr\
    -in $stageTab.fLinearStage1\
    -row 3\
    -column 0

grid $stageTab.fLinearStage1.fLinear1PosErr\
    -in $stageTab.fLinearStage1\
    -row 3\
    -column 1

grid $stageTab.fLinearStage1.fLinear1StageState\
    -in $stageTab.fLinearStage1\
    -row 3\
    -column 2



source "$MAINPATH/commands/C120_HomeLinear2.tcl"
source "$MAINPATH/commands/C121_AbsMoveLinear2.tcl"




ttk::label $stageTab.fLinearStage2.tLinearStage\
    -text "Position (mm):"\
    -anchor center\


ttk::label $stageTab.fLinearStage2.tLinearStageAngle\
    -textvariable var::stage2pos\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage2.fLinear2StageErr\
    -textvariable var::stage2err\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage2.fLinear2PosErr\
    -textvariable var::pos2err\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage2.fLinear2StageState\
    -textvariable var::stage2stat\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center


grid $stageTab.fLinearStage2.tLinearStage\
    -in $stageTab.fLinearStage2\
    -row 0\
    -column 1\
    -padx $positions::padX\
    -pady $positions::padY

grid $stageTab.fLinearStage2.tLinearStageAngle\
    -in $stageTab.fLinearStage2\
    -row 0\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY



grid $stageTab.fLinearStage2.fLinear2StageErr\
    -in $stageTab.fLinearStage2\
    -row 3\
    -column 0

grid $stageTab.fLinearStage2.fLinear2PosErr\
    -in $stageTab.fLinearStage2\
    -row 3\
    -column 1

grid $stageTab.fLinearStage2.fLinear2StageState\
    -in $stageTab.fLinearStage2\
    -row 3\
    -column 2


source "$MAINPATH/commands/C130_HomeLinear3.tcl"
source "$MAINPATH/commands/C131_AbsMoveLinear3.tcl"




ttk::label $stageTab.fLinearStage3.tLinearStage\
    -text "Position (mm):"\
    -anchor center\


ttk::label $stageTab.fLinearStage3.tLinearStageAngle\
    -textvariable var::stage3pos\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage3.fLinear3StageErr\
    -textvariable var::stage3err\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage3.fLinear3PosErr\
    -textvariable var::pos3err\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

ttk::label $stageTab.fLinearStage3.fLinear3StageState\
    -textvariable var::stage3stat\
    -justify left\
    -width 10\
    -relief solid\
    -anchor center

grid $stageTab.fLinearStage3.tLinearStage\
    -in $stageTab.fLinearStage3\
    -row 0\
    -column 1\
    -padx $positions::padX\
    -pady $positions::padY

grid $stageTab.fLinearStage3.tLinearStageAngle\
    -in $stageTab.fLinearStage3\
    -row 0\
    -column 2\
    -padx $positions::padX\
    -pady $positions::padY



grid $stageTab.fLinearStage3.fLinear3StageErr\
    -in $stageTab.fLinearStage3\
    -row 3\
    -column 0

grid $stageTab.fLinearStage3.fLinear3PosErr\
    -in $stageTab.fLinearStage3\
    -row 3\
    -column 1

grid $stageTab.fLinearStage3.fLinear3StageState\
    -in $stageTab.fLinearStage3\
    -row 3\
    -column 2

source "$MAINPATH/commands/C132_FocusTipTilt.tcl"
source "$MAINPATH/commands/C137_FocusDelta.tcl"

source "$MAINPATH/commands/C135_CarouselPosition.tcl"
