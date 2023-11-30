namespace eval positions {

set padX 5 
set padY 5

# Open Shutter A
set P080On 1
set P080Tab $lampTab
set P080X 0 
set P080Y 0

# Close Shutter A
set P081On $P080On
set P081Tab $P080Tab
set P081X 0  
set P081Y 1

# Shutter A status
set ShutterATab $lampTab
set ShutterALFrame $ShutterATab.shutterA
set ShutterAX 0
set ShutterAY 1

# --------------------------------------
# Stage functions 
set P100On 1
set RotaryStageTab $stageTab
set RotaryStageFrame $stageTab.RotaryStage
set RotaryStageX 0
set RotaryStageY 0

set P132On 1
set FocusTipTiltTab $stageTab
set FocusTipTiltX 0
set FocusTipTiltY 6

set P136On 1
set FocusNominalTab $stageTab
set FocusNominalX 0
set FocusNominalY 7

set P135On 1
set tCarouselPosition $stageTab
set CarouselPositionX 0
set CarouselPositionY 8


set P101X 0
set P101Y 1

set P110On 1
set LinearStageTab1 $stageTab
set LinearStageFrame1 $stageTab.LinearStage
set LinearStage1X 0
set LinearStage1Y 1

set P111X 0
set P111Y 1

#------------------------------------------------
set P110On 1
set LinearStageTab2 $stageTab
set LinearStageFrame2 $stageTab.LinearStage
set LinearStage2X 0
set LinearStage2Y 2

set P111X 0
set P111Y 1

set P110On 1
set LinearStageTab3 $stageTab
set LinearStageFrame3 $stageTab.LinearStage
set LinearStage3X 0
set LinearStage3Y 3

set P111X 0
set P111Y 1





#------------------------------------------------


#------------------------------------------------
set tExposureTime $cameraTab
set tGuiderMode   $cameraTab
set tDisplayMode   $cameraTab
set tDisplayStretch   $cameraTab
set tChangeSubtract $cameraTab
set tStackSolve    $cameraTab
set tNTtoS $targetTab
set tNTtoSS $targetTab
set tAdjustPointing $computerTab
set tAdjustStar $computerTab
set tSwitchStar $targetTab
set tLoadTarget $targetTab
set tSetImageTag $infoTab
set tSwitchNext $targetTab
set tNewTarget $targetTab
set tChangeUsed $targetTab
set tChangeGuideSigma $targetTab
set tChangeAlgorithm $cameraTab
set tStarBoxCut $cameraTab
set tSavePeriod $cameraTab
set tGondolaBoresight $computerTab
set tRefineTargets $computerTab
set tCollectMin $cameraTab
set tDither $computerTab
set LinearDelta $stageTab
}
