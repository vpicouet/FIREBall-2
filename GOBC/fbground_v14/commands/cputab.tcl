namespace eval cputab {
	  set tab $computerTab

    ttk::labelframe $tab.objframe\
	      -text "Targets and Stars"

    
    pack $tab.objframe\
	      -in $tab


	ttk::label $tab.objframe.obj\
	    -text "OBJ"

	ttk::label $tab.objframe.tx\
	    -text "TX"

	ttk::label $tab.objframe.ty\
	    -text "TY"

	ttk::label $tab.objframe.cx\
	    -text "CX"

	ttk::label $tab.objframe.cy\
	    -text "CY"

	ttk::label $tab.objframe.f\
	    -text "F"

	ttk::label $tab.objframe.sx\
	    -text "SigX"

	ttk::label $tab.objframe.sy\
	    -text "SigY"


	ttk::label $tab.objframe.val\
	    -text "VAL"

	ttk::label $tab.objframe.use\
	    -text "USE"
	ttk::label $tab.objframe.mag\
	    -text "GMAG?"

	grid $tab.objframe.obj\
	    -in $tab.objframe\
	    -row 0\
	    -column 1\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.tx\
	    -in $tab.objframe\
	    -row 0\
	    -column 2\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.ty\
	    -in $tab.objframe\
	    -row 0\
	    -column 3\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.cx\
	    -in $tab.objframe\
	    -row 0\
	    -column 4\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.cy\
	    -in $tab.objframe\
	    -row 0\
	    -column 5\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.sx\
	    -in $tab.objframe\
	    -row 0\
	    -column 6\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.sy\
	    -in $tab.objframe\
	    -row 0\
	    -column 7\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.f\
	    -in $tab.objframe\
	    -row 0\
	    -column 8\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.val\
	    -in $tab.objframe\
	    -row 0\
	    -column 9\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.use\
	    -in $tab.objframe\
	    -row 0\
	    -column 10\
	    -padx 3\
	    -pady 3

# Adding magnitude column
	grid $tab.objframe.mag\
	    -in $tab.objframe\
	    -row 0\
	    -column 11\
	    -padx 3\
	    -pady 3
    

    # txN, tyN, cxN, cyN, useN, validN
    
    for {set i 0 } { $i < 8 } { incr i } {
	
	ttk::label $tab.objframe.obj$i\
	    -text "$i"
	
	ttk::label $tab.objframe.tx$i\
	    -textvariable var::tx$i

	ttk::label $tab.objframe.ty$i\
	    -textvariable var::ty$i

	ttk::label $tab.objframe.cx$i\
	    -textvariable var::cx$i

	ttk::label $tab.objframe.cy$i\
	    -textvariable var::cy$i

	ttk::label $tab.objframe.f$i\
	    -textvariable var::f$i

	ttk::label $tab.objframe.valid$i\
	    -textvariable var::valid$i

	ttk::label $tab.objframe.use$i\
	    -textvariable var::use$i

	ttk::label $tab.objframe.sx$i\
	    -textvariable var::sx$i

	ttk::label $tab.objframe.sy$i\
	    -textvariable var::sy$i

	ttk::label $tab.objframe.mag$i\
	    -textvariable var::mag$i


	set row [expr $i+1]


	grid $tab.objframe.obj$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 1\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.tx$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 2\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.ty$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 3\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.cx$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 4\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.cy$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 5\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.sx$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 6\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.sy$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 7\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.f$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 8\
	    -padx 3\
	    -pady 3




	grid $tab.objframe.valid$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 9\
	    -padx 3\
	    -pady 3

	grid $tab.objframe.use$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 10\
	    -padx 3\
	    -pady 3
	grid $tab.objframe.mag$i\
	    -in $tab.objframe\
	    -row $row\
	    -column 11\
	    -padx 3\
	    -pady 3
	

    };
	

};