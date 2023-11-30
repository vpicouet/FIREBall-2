namespace eval infopanel { 
    set tab $infoTab

    ttk::labelframe $tab.modeframe\
	-text "Modes"

    ttk::label $tab.modeframe.guidermode\
	-text "Guider Mode:"\
	-justify left

    ttk::label $tab.modeframe.guidermodevar\
	-textvariable var::guider_mode\
	-width 10\
	-justify left

    ttk::label $tab.modeframe.displaymode\
	-text "Display Mode:"\
	-justify left

    ttk::label $tab.modeframe.displaymodevar\
	-textvariable var::display_mode\
	-width 10\
	-justify left

    ttk::labelframe $tab.camframe\
	-text "Camera"

    ttk::label $tab.camframe.exptime\
	-text "Exposure time (ms): "\
	-justify left

    ttk::label $tab.camframe.exptimevar\
	-textvariable var::exposure_time\
	-width 10\
	-justify left

    ttk::label $tab.camframe.framenumber\
	-text "Frame Number: "\
	-justify left

    ttk::label $tab.camframe.framenumbervar\
	-textvariable var::frame_number\
	-width 10\
	-justify left



    ttk::labelframe $tab.temperatureframe\
	-text "Temperatures"
    
    ttk::label $tab.temperatureframe.ccdtemp\
	-text "CCDT(C):"\
	-justify left

    ttk::label $tab.temperatureframe.ccdtempvar\
	-textvariable var::ccd_temperature\
	-justify left

    ttk::label $tab.temperatureframe.camtemp\
	-text "CamT (C):"\
	-justify left

    ttk::label $tab.temperatureframe.camtempvar\
	-textvariable var::cam_temperature\
	-justify left

    ttk::label $tab.temperatureframe.pstemp\
	-text "PST (C):"\
	-justify left

    ttk::label $tab.temperatureframe.pstempvar\
	-textvariable var::ps_temperature\
	-justify left

    ttk::label $tab.temperatureframe.aitemp\
        -text "T (C):"\
        -justify left

    ttk::label $tab.temperatureframe.aitempvar\
        -textvariable var::ai_temperature\
        -justify left

    ttk::label $tab.temperatureframe.aipres\
        -text "P (mbar):"\
        -justify left

    ttk::label $tab.temperatureframe.aipresvar\
        -textvariable var::ai_pressure\
        -justify left



    grid $tab.modeframe\
	-in $tab\
	-row 0

    grid $tab.camframe\
	-in $tab\
	-row 1
    
    grid $tab.temperatureframe\
	-in $tab\
	-row 2
    
    grid $tab.modeframe.guidermode\
	-in $tab.modeframe\
	-row 0\
	-column 0

    grid $tab.modeframe.guidermodevar\
	-in $tab.modeframe\
	-row 0\
	-column 1

    grid $tab.modeframe.displaymode\
	-in $tab.modeframe\
	-row 1\
	-column 0

    grid $tab.modeframe.displaymodevar\
	-in $tab.modeframe\
	-row 1\
	-column 1

    grid $tab.camframe.exptime\
	-in $tab.camframe\
	-row 0\
	-column 0

    grid $tab.camframe.exptimevar\
	-in $tab.camframe\
	-row 0\
	-column 1

    grid $tab.camframe.framenumber\
	-in $tab.camframe\
	-row 1\
	-column 0

    grid $tab.camframe.framenumbervar\
	-in $tab.camframe\
	-row 1\
	-column 1

    grid $tab.temperatureframe.ccdtemp\
	-in $tab.temperatureframe\
	-row 0\
	-column 0

    grid $tab.temperatureframe.ccdtempvar\
	-in $tab.temperatureframe\
	-row 0\
	-column 1

    grid $tab.temperatureframe.camtemp\
	-in $tab.temperatureframe\
	-row 1\
	-column 0

    grid $tab.temperatureframe.camtempvar\
	-in $tab.temperatureframe\
	-row 1\
	-column 1

    grid $tab.temperatureframe.pstemp\
	-in $tab.temperatureframe\
	-row 2\
	-column 0

    grid $tab.temperatureframe.pstempvar\
	-in $tab.temperatureframe\
	-row 2\
	-column 1



   grid $tab.temperatureframe.aitemp\
        -in $tab.temperatureframe\
        -row 3\
        -column 0

    grid $tab.temperatureframe.aitempvar\
        -in $tab.temperatureframe\
        -row 3\
        -column 1

  grid $tab.temperatureframe.aipres\
        -in $tab.temperatureframe\
        -row 4\
        -column 0

    grid $tab.temperatureframe.aipresvar\
        -in $tab.temperatureframe\
        -row 4\
        -column 1

   


}