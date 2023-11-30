# 

proc savestuff { } { 
    # start a file
    set nowtime [clock format [clock seconds] -timezone :UTC \
		     -format %y%m%d-%H%M%S]
    set filename "./saves/$nowtime.state"
    set ofile [open $filename "w"]
    
    # 
    # guider information
    #
    puts $ofile "General information"
    puts $ofile "GuiderMode = $var::guider_mode"
    puts $ofile "DisplayMode = $var::display_mode"
    puts $ofile "FrameNumber = $var::frame_number"
    puts $ofile "ExposureTime = $var::exposure_time"
    puts $ofile "SavingPeriod = $var::saveperiod"
    puts $ofile "ImageTag = $var::imagetag"

    #
    # temperature and pressure
    # 
    puts $ofile "Temperature and pressure"
    puts $ofile "CCDTemp = $var::ccd_temperature"
    puts $ofile "CAMTemp = $var::cam_temperature"
    puts $ofile "PSTemp = $var::ps_temperature"
    puts $ofile "AITemp = $var::ai_temperature"
    puts $ofile "AIPress = $var::ai_pressure"

    #
    # tracking information   
    #
    puts $ofile "Tracking information"
    for { set i 0 } { $i < 8 } { incr i } {
	set vr var::use$i
	puts $ofile "USE$i = [expr $$vr]"

	set vr var::valid$i
	puts $ofile "VALID$i = [expr $$vr]"

	set vr var::cx$i
	puts $ofile "XCENT$i = [expr $$vr]"

	set vr var::cy$i
	puts $ofile "YCENT$i = [expr $$vr]"

	set vr var::tx$i
	puts $ofile "XTARG$i = [expr $$vr]" 

	set vr var::ty$i
	puts $ofile "YTARG$i = [expr $$vr]" 

	set vr var::f$i
	puts $ofile "FLUX$i = [expr $$vr]"

	set vr var::sx$i
	puts $ofile "XSIGMA$i = [expr $$vr]"

	set vr var::sy$i
	puts $ofile "YSIGMA$i = [expr $$vr]"
    };


    # camera information
    #
    # gondola information
    # 
    # temperatures and the like 
    # 
    # lamp and shutter status
    # 
    puts $ofile "Lamp + Shutter Parameters"
    puts $ofile "shutterA = $var::shutAstatus"
    puts $ofile "shutterB = $var::shutBstatus"
    puts $ofile "shutterC = $var::shutCstatus"
    puts $ofile "shutterD = $var::shutDstatus"
    puts $ofile "lampA = $var::lampAstatus"
    puts $ofile "lampB = $var::lampBstatus"
    # 
    # stage information
    # 
    puts $ofile "Stage Positions"
    puts $ofile "stageR = $var::stagerpos"
    puts $ofile "stageA = $var::stagerpos"
    puts $ofile "stageB = $var::stagerpos"
    puts $ofile "stageC = $var::stagerpos"
    # 
    # frameblob parameters
    #
    puts $ofile "FRAMEBLOB Parameters"
    puts $ofile "FBGain = $C221::fb"
    puts $ofile "FBSatVal = $C222::fb"
    puts $ofile "FBThresh = $C223::fb"
    puts $ofile "FBDistTol = $C224::fb"
    puts $ofile "FB = $C225::fb"
    puts $ofile "FB = $C226::fb"
    puts $ofile "FB = $C227::fb"
    puts $ofile "FB = $C228::fb"
    puts $ofile "FB = $C229::fb"
    
    close $ofile
}