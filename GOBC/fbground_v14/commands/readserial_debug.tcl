namespace eval readserial  {

    variable buffer "."


    variable startb [ binary format c 0x55 ]
    variable endb [ binary format c 0xAA ]
    variable count 0
    variable lastcnt 0


    proc test {message} {
	variable startb
	variable endb
	

    }

    proc parse {message} {
	variable lastcnt 
#	puts "Process called with $message"
	set lgth [string length $message]
	for { set i 0 } { $i < $lgth } { incr i } {
	    set blah [string index $message $i]
	    scan $blah %c charblah
	}
#	puts " "
	binary scan $message cssc sb ct len mid
	# convert to unsigned values
	set ct [expr $ct & 0xFFFF ]
	set len [expr $len & 0xFFFF ]
	set mid [expr $mid & 0xFF ]

	puts "use: $ct $len $mid"

	set data [string range $message 6 end-2]
	set crc [string range $message end-1 end-1]
	puts "Parsing..."

	if {$ct - $lastcnt != 1} {
	    
	    .lb insert end  "Some messages have been skipped. now=$ct, last=$lastcnt"
	}
	#update message counter.
	set lastcnt $ct

	set mid [expr  $mid & 0xFF]
	switch -- $mid {
	    128 {
		binary scan $data sssssssssssssssssssssssscccccccccccccccccc \
		    cx0 cy0 f0 cx1 cy1 f1 cx2 cy2 f2 cx3 cy3 f3 cx4 cy4 f4 \
		    cx5 cy5 f5 cx6 cy6 f6 cx7 cy7 f7 \
		    sx0 sy0 sx1 sy1 sx2 sy2 sx3 sy3 sx4 sy4 sx5 sy5 sx6 sy6 \
		    sx7 sy7 uses valids
		set validall 0
		set usedall 0
		set pipestr "RCVD $ct 128 " 
		for { set i 0 } { $i < 8 } { incr i } {
		    set cx cx$i
		    set cx [ expr ($$cx & 0xffff) / 50.0 ]
		    set cy cy$i
		    set cy [ expr ($$cy & 0xffff) / 50.0 ]
		    set f f$i
		    set f [ expr ($$f & 0xffff) ]
		    set sx sx$i
		    set sy sy$i
		    set sx [ expr ($$sx & 0xff) / 20.0 ]
		    set sy [ expr ($$sy & 0xff) / 20.0 ]
		    #magnitude calculation -- VERY rough estimate, email Meghna with questions?
		    #set expo $var::exposure_time
		    #puts "exposure time $expo"
		    #puts $f
		    #if {$expo > 0 && $f > 0} {set mag [ expr {-2.5 * log10( $f / $expo) + 20.2 } ]}
		    #else {
		    set mag 0.0
		    #}
		    set mag [format {%.2f} $mag]
		    #set mag 0.0
		    set var::sx$i [format {%.2f} $sx]
		    set var::sy$i [format {%.2f} $sy]

		    set var::cx$i [format {%6.1f} $cx]
		    set var::cy$i [format {%6.1f} $cy]
		    set var::f$i $f
		    set var::mag$i $mag
		    set usedall [expr $usedall + ( ( $uses >> $i ) & 1) ]
		    set use [expr ( $uses >> $i) & 1 ]
		    set validall [expr $validall + ( ( $valids >> $i ) &1) ]
		    set valid [expr ( $valids >> $i ) & 1]
		    set var::use$i $use
		    set var::valid$i $valid
		    set var::totused $usedall
		    set var::totvalid $validall

		    set infostr "$cx $cy $sx $sy $f"
		    append pipestr $infostr
		}
		set str "RCVD $ct: 128 $var::cx0 $var::cy0 $var::cx1 $var::cy1 $var::cx2 $var::cy2 $var::cx3 $var::cy3 $var::cx4 $var::cy4  $var::cx5 $var::cy5 $var::cx6 $var::cy6 $var::cx7 $var::cy7 $var::sx0 $var::sy0 $uses $valids"
		    writelog $str
		#set pipestr "RCVD $ct: 128 $var::cx0 $var::cy0 $var::cx1 $var::cy1 $var::cx2 $var::cy2 $var::cx3 $var::cy3 $var::sx0 $var::sy0 $var::sx1 $var::sy1 $var::sx2 $var::sy2 $var::sx3 $var::sy3 $uses $valids"
		append pipestr $var::exposure_time $uses $valids
		writepipe $pipestr



		updatestars
	    }

	    129 {
		binary scan $data sssssssssssssssscss tx0 ty0 tx1 ty1 tx2 ty2 tx3 ty3 tx4 ty4 tx5 ty5 tx6 ty6 tx7 ty7 thresh smin smax
		for { set i 0 } { $i < 8 } { incr i } {
		    set cx tx$i
		    set cx [ expr ($$cx & 0xffff) / 50.0 ]
		    set cy ty$i
		    set cy [ expr ($$cy & 0xffff) / 50.0 ]
		    set var::tx$i [format {%.1f} $cx]
		    set var::ty$i [format {%.1f} $cy]
		}
		set var::thresh [expr ($thresh & 0xff)/16.0]
		set var::smin [expr ($smin & 0xffff)/2048.0]
		set var::smax [expr ($smax & 0xffff)/2048.0]
#		.lb insert end $str
#		.lb itemconfigure [expr [.lb index end] -1] -foreground green
#		.lb see [expr [.lb index end]]
		set str "RCVD $ct: 129 $var::tx0 $var::ty0 $var::tx1 $var::ty1 $var::tx2 $var::ty2 $var::tx3 $var::ty3 $var::tx4 $var::ty4  $var::tx5 $var::ty5 $var::tx6 $var::ty6 $var::tx7 $var::ty7 $var::thresh $var::smin $var::smax"
		writelog $str
		updatetargets
	    }

	    252 {
		binary scan $data cccissssciccfffssssssf gm gsm as fn rx ry rdx rdy tt et dac fl glat glon galt wx wy wdx wdy tx ty lvdt
		set gm [expr $gm & 0xff]
		set gsm [expr $gsm & 0xff]
		set as [expr $as & 0xff]
		set fn [expr $fn & 0xffffffff]
		set rx [expr $rx & 0xffff]
		set ry [expr $ry & 0xffff]
		set rdx [expr $rdx & 0xffff]
		set rdy [expr $rdy & 0xffff]
		set tt [expr $tt & 0xff]
		set et [expr $et & 0xffffffff]
		set dac [expr $dac & 0xff]
		set fl [expr $fl & 0xff]
		set wx [expr $wx & 0xffff]
		set wy [expr $wy & 0xffff]
		set wdx [expr $wdx & 0xffff]
		set wdy [expr $wdy & 0xffff]
		set tx [expr $tx & 0xffff]
		set ty [expr $ty & 0xffff]

		
		
		set var::md $gm
		set var::sm $gsm
		set var::as $as

		set var::framex $rx
		set var::framey $ry
		set var::framedx $rdx
		set var::framedy $rdy
		
		set var::framett $tt
		set var::frameet $et
		
		set var::gpslat $glat
		set var::gpslon $glon
		set var::gpsalt $galt

		set gpsdat [open "gps.dat" "w"]
		
		close $gpsdat


		set var::wx $wx
		set var::wy $wy
		set var::wdx $wdx
		set var::wdy $wdy

		set var::tx $tx
		set var::ty $ty

		set var::dac $dac
		set var::led $fl
		set var::lvdt $lvdt
		
		set var::lvdtvolt [expr 1.0 * $lvdt / 4096.0 * 10.0]

		 set str "RCVD $ct: 252: $gm, $gsm, $as, $fn, $rx, $ry, $rdx, $rdy, $tt, $et, $dac, $fl, $glat, $glon, $galt, $wx, $wy, $wdx, $wdy, $tx, $ty $lvdt"

		updatefltrackpoint $tx $ty
		updateroi $rx $ry $rdx $rdy
		updatewin $wx $wy $wdx $wdy
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }

	    254 { 
		binary scan $data c response
		set response [expr $response & 0xff]
		set str "RCVD $ct: 254: $response (ECHO)" 
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground purple
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }


	    251 { 
		binary scan $data fff acx acy acr
		set var::acx $acx
		set var::acy $acy
		set var::acr $acr
	    }
	    249 { 
		set str "RCVD $ct $mid"
		puts $str
#		puts "+++++++++++++++++++++++++++++++++++++++++++++++"
		binary scan $data ci mask frameno
		set starid [expr $mask & 15 ]
#		puts $starid 
#		puts [expr ( $mask & 16) > 0]
#		puts [expr ($mask & 32)>0 ] 
#		puts $frameno
		set fname [format "./profiles/%1d_%07d_prof.raw" $starid $frameno]
#		puts $fname
		set profraw [open $fname "w"]
		puts -nonewline $profraw $data
		close $profraw
		write_profile $data
#		puts "+++++++++++++++++++++++++++++++++++++++++++++++"
		writelog $str
	    }
            244 {
                binary scan $data fff ra dec roll
                puts "RCVD 244 $ra $dec $roll"
                set var::gra [ format { %9.3f } $ra ]
		set var::gdec [ format { %9.3f } $dec ]
		set var::groll [ format { %9.3f } $roll ]
                set str "RCVD $mid $var::gra $var::gdec $var::groll"
                writelog $str
            }
	    245 { 
		binary scan $data fff az el rot
		puts "RCVD 245 $az $el $rot"
		set var::gaz [ format { %9.2f} $az  ]
		set var::gel [format { %9.2f} $el ]
		set var::grot [format { %9.2f} $rot ]
		set str "RCVD $mid $var::gaz $var::gel $var::grot"
		writelog $str
	    }
	    248 { 
		binary scan $data ffffccccsssscccc stagerpos stage1pos stage2pos stage3pos stagererr stage1err stage2err stage3err posrerr pos1err pos2err pos3err stagerstat stage1stat stage2stat stage3stat
		set var::stagerpos [format {%8.3f} $stagerpos]
		set var::stage1pos [format {%8.3f}  $stage1pos ]
		set var::stage2pos [format {%8.3f}  $stage2pos ]
		set var::stage3pos [format {%8.3f}  $stage3pos ]
		set var::stagererr [format {%c} [expr  ($stagererr & 0xFF)]]
		set var::stage1err [format {%c} [expr ( $stage1err & 0xFF)]]
		set var::stage2err [format {%c} [expr ( $stage2err & 0xFF)]]
		set var::stage3err [format {%c} [expr ( $stage3err & 0xFF)]]

		set var::posrerr [format {%X} [expr ( $posrerr & 0xFFFF)]]
		set var::pos1err [format {%X} [expr ( $pos1err & 0xFFFF)]]
		set var::pos2err [format {%X} [expr ( $pos2err & 0xFFFF)]]
		set var::pos3err [format {%X} [expr ( $pos3err & 0xFFFF)]]

		set var::stagerstat [format {%X} [expr ( $stagerstat & 0xFF)]]
		set var::stage1stat [format {%X} [expr ( $stage1stat & 0xFF)]]
		set var::stage2stat [format {%X} [expr ( $stage2stat & 0xFF)]]
		set var::stage3stat [format {%X} [expr ( $stage3stat & 0xFF)]]
		set response "stagepos: $stagerpos $stage1pos $stage2pos $stage3pos $stagererr $stage1err $stage2err $stage3err $posrerr $pos1err $pos2err $pos3err $stagerstat $stage1stat $stage2stat $stage3stat"
		set str "RCVD $ct: 248: $response" 
#		.lb2 insert end $str
#		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground green
#		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }




	    #camera error.
	    253 {
		binary scan $data c err
		set err [expr $err & 0xff]
		set str "RCVD Camera error \# $err."
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end]-1] -foreground red
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }

	    #text error.
	    124 { 
		set str "RCVD \"$data\"."
#		puts $str
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground red
		.lb2 see [expr [.lb2 index end]]
		writelog $str
	    }

	    246 {
		binary scan $data c diobyte
		set str "RCVD $ct: $mid $diobyte"
		writelog $str
#		.lb insert end $str
#		.lb itemconfigure [expr [.lb index end] -1] -foreground blue 
#		.lb see [expr [.lb index end]]

		set dio [expr ( ( $diobyte & 0x01 ) > 0 )  ]
		if {$dio > 0} {
		    set var::shutAstatus "Not Used (Open)"
		} else {
		    set var::shutAstatus "Not Used (Closed)"
		}
		#		set var::shutAstatus [ expr ( ( $diobyte & 0x01 ) > 0 )  ]
		
		set dio [expr ( ( $diobyte & 0x02 ) > 0 )  ]
		if {$dio > 0} {
		    set var::shutBstatus Closed
		} else {
		    set var::shutBstatus Open
		}
		#		set var::shutBstatus [ expr ( ( $diobyte & 0x02 ) > 0 ) ]
		
		set dio [expr ( ( $diobyte & 0x04 ) > 0 )  ]
		if {$dio > 0} {
		    set var::shutCstatus "Open"
		} else {
		    set var::shutCstatus "Closed"
		}
		#		set var::shutCstatus [ expr ( $diobyte & 0x04 )>0 ]
		
		set dio [expr ( ( $diobyte & 0x08 ) > 0 )  ]
		if {$dio > 0} {
		    set var::shutDstatus Open
		} else {
		    set var::shutDstatus Closed
		}
		#		set var::shutDstatus [ expr ( $diobyte & 0x08 )>0 ]
	   
		set dio [expr ( ( $diobyte & 0x10 ) > 0 )  ]
		if {$dio > 0} {
		    set var::lampAstatus On
		} else {
		    set var::lampAstatus Off
		}
#		set var::lampAstatus [ expr ( $diobyte & 0x10 )>0 ]
		set dio [expr ( ( $diobyte & 0x40 ) > 0 )  ]
		if {$dio > 0} {
		    set var::lampBstatus On
		} else {
		    set var::lampBstatus Off
		}
#		set var::lampBstatus [ expr ( $diobyte & 0x40 )>0 ]
	    }
	    247 { 
		binary scan $data ccccisuisssffffffs gm dm alg sub fn tag et ccdt camt pst ait aip aigt aigp aict aicp savep
		set str "RCVD $ct: 247 $gm $dm $alg $sub $fn $tag $et $ccdt $camt $pst $ait $aip $aigt $aigp $aict $aicp $savep"
		writelog $str


#		.lb insert end $str
#		.lb itemconfigure [expr [.lb index end] -1] -foreground blue 
#		.lb see [expr [.lb index end]]

		set var::guider_mode $gm
		set var::display_mode $dm

		set var::algorithm [expr ( $alg & 0xFF)]
		set var::subtract [expr ( $sub & 0xFF)]
		set var::imagetag [expr ( $tag & 0xFFFF)]

		set var::frame_number $fn
		set var::exposure_time $et
		set var::ccd_temperature [expr $ccdt*0.1]
		set var::cam_temperature $camt
		set var::ps_temperature $pst

#		set var::stagerpos [format {%8.3f} $stagerpos]
#		set var::ai_temperature $ait
		set var::ai_temperature [format {%.1f} $ait]
#		set var::ai_pressure $aip
		set var::ai_pressure [format {%.1f} $aip]


		set var::ai_gobc_temp [format {%.1f} $aigt]
		set var::ai_gobc_pressure [format {%.1f} $aigp]
		set var::ai_cal_temp [format {%.1f} $aict]
		set var::ai_cal_pressure [format {%.1f} $aicp]

		set var::saveperiod $savep
	    }
	    250 {
		
		binary scan $data cs2is2is2is2is2is2is2is2is2is2i n b0 f0 b1 f1 b2 f2 b3 f3 b4 f4 b5 f5 b6 f6 b7 f7 b8 f8 b9 f9
		set str "RCVD $ct: 250: $n $b0 $f0 $b1 $f1 $b2 $f2 $b3 $f3 $b4 $f4 $b5 $f5 $b6 $f6 $b7 $f7 $b8 $f8 $b9 $f9"

		writelog $str
		
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		if { $n > 0 } {
		    set var::bcnt $n
		    for {set i 0 } { $i < 10 } { incr i } {
			if {$i < $n} { 
			    set bx b$i
			    set bx [expr $$bx]
			    set f f$i
			    set f [expr $$f]
			    set var::bx$i [lindex $bx 0]
			    set var::by$i [lindex $bx 1]
			    set var::bf$i $f
			    

			} else {
			    set var::bx$i 0
			    set var::by$i 0
			    set var::bf$i 0
			}

		    };
		    
		} else {
		    set bcnt 0
		}
		updateblobs
	    }
	    
	    255 {
		binary scan $data ff el ce
		

		set str "RCVD $ct: 255: $el, $ce"
		.lb2 insert end $str
		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		.lb2 see [expr [.lb2 index end]]
		writelog $str


		set var::meanel [expr 0.98*$var::meanel + 0.02*$el]
		set var::meance [expr 0.98*$var::meance + 0.02*$ce]
		set var::meanrmsel [expr sqrt(0.98*pow($var::meanrmsel,2)+ 0.02*pow($var::meanel - $el,2)) ]
		set var::meanrmsce [expr sqrt(0.98*pow($var::meanrmsce,2)+ 0.02* pow($var::meance - $ce,2)) ]

	    }

	    230 { 
		binary scan $data sssssssss gain satval thresh distol gridsize cenbox apbox offset rn 
		set C221::fbval [ expr ( $gain & 0xffff ) / 2048.0 ]
		set C222::fbval [ expr ( $satval & 0xffff ) ]
		set C223::fbval [ expr ( $thresh & 0xffff ) /2048.0 ]
		set C224::fbval [ expr ( $distol ) ]
		set C225::fbval [ expr ( $gridsize & 0xffff ) ]
		set C226::fbval [ expr ( $cenbox & 0xffff ) ]
		set C227::fbval [ expr ( $apbox & 0xffff ) ]
		set C228::fbval [ expr ( $offset ) ]
		set C229::fbval [ expr ( $rn & 0xffff ) /2048 ]
		
		set str "RCVD: $ct: $mid: $C221::fbval $C222::fbval\ $C223::fbval $C224::fbval $C225::fbval $C226::fbval $C227::fbval $C228::fbval $C229::fbval"
		#		.lb2 insert end $str
		#		.lb2 itemconfigure [expr [.lb2 index end] -1] -foreground blue 
		#		.lb2 see [expr [.lb2 index end]]
		writelog $str		

	    }

	    240 {
		binary scan $data ffffff dx dy dr rx ry rr

		if { $dx == NaN } {
		    set dx 0.0
		}
		if { $dy == NaN } {
		    set dy 0.0
		}
		if { $dr == NaN } {
		    set dr 0.0
		}
		set var::dx [ format {%0.2f} $dx ]
		set var::dy [ format {%0.2f} $dy ]
		set var::dr [ format {%0.3f} $dr ]
		set var::rmsdx [ format {%0.3f} $rx ]
		set var::rmsdy [ format {%0.3f} $ry ]
		set var::rmsdr [ format {%0.4f} $rr ]
	    }

	    default {
		if { $mid != 0 } {
		set str "RCVD: unknown message $mid "
		     .lb2 insert end $str
		     .lb2 itemconfigure [ expr [ .lb2 index end ]  -1 ] -foreground blue 
		    .lb2 see [expr [ .lb2 index end ] ]
		     puts "BAD COMMAND $str"
		     writelog $str
		}
	    }


	}
	set mid 0 
	
    }

    proc parsebuffer {data} { 
	variable buffer
	variable startb
	variable endb
	variable count


	#add the new bytes to the buffer
	set buffer $buffer$data

	set start [ string first $startb $buffer ]




	# if there is no start character, we can't possibly have a valid message. 
	if { $start == -1 } {
	    set buffer "."
	    return
	} 



	
	# let's get rid of the leading useless characters. 
	set buffer [string range $buffer $start end]
	set start 0

	# if the buffer size is less than 8 characters, we can't have a valid message. 
	# there are a total of 7 control bytes, and the smallest possible data 
	# message is 1 byte. In this case just sit there and wait for more. 
	if { [string length $buffer] < 8  } {
	    return
	}


	binary scan $buffer cssc sb count length id 
	puts "$count $id"

	# if the buffer is shorter than the length indicated in the message, then we can't 
	# have a full message there. should exit and await more input.

	puts "message ([expr $id & 0xFF])  length : $length"

	set lb [string length $buffer]

#	binary scan $buffer c$lb vals
#	puts $vals


	if { [string length $buffer] < [expr 7 + $length]} {
	    puts "Message not quite long enough, actually"
	    return
	}

	# so we have enough characters for this to be a message. Is the last character the 
	# appropriate end-byte synchbyte? If it is, cut it out to be a message and do some
	# more processing with it. If it isn't then we can't have a valid message starting
	# where 
	
	set blah [string index $buffer [expr 6+$length]]
	scan $blah %c charblah
	scan $endb %c charendb
	puts "$charblah $charendb"
	if { [string index $buffer [expr 6+$length]] == $endb } {
	    set message [string range $buffer 0 [expr 6+$length]]
	    binary scan $message cssc aa bb cc mid
	    set bb [expr {$bb & 0xFFFF}]
	    set mid [expr {$mid & 0xFF}]
	    set buffer [string range $buffer [expr 6+$length] end]
	    parse $message
	    parsebuffer ""

	} else {
	    set buffer [string range $buffer 1 end]
	    parsebuffer ""
	}
	    
    }

    proc write_profile { data } {
	binary scan $data ci mask frameno
	set a [string length $data]
	puts "Length: $a"
	
	## c structure being parsed
	# typedef struct star_profile_t{
	# unsigned char starmask;
	# long framenumber;
	# unsigned short xprofile[IMG_BOXSIZE*2+1];
	# unsigned short yprofile[IMG_BOXSIZE*2+1];
	# float xfit[4];
	# float yfit[4];
	# }star_profile_t;
	set profsize [expr (($a-1-4-16-16-16-5)/2/2) ]
#	puts "Profsize: $profsize"

	set scanstr cis${profsize}s${profsize}fffffffff4ssc
	binary scan $data $scanstr starmask frameno xprofile yprofile xfit0 xfit1 xfit2 xfit3  yfit0 yfit1 yfit2 yfit3 motorpos cx cy ct

#	puts "xfit $xfit0 $xfit1 $xfit2 $xfit3"
#	puts "yfit $yfit0 $yfit1 $yfit2 $yfit3"

	set starid [expr ($starmask & 15)]
#	set cx [expr $cx +($profsize-1)/2 + $xfit1]
#	set cy [expr $cy +($profsize-1)/2 + $yfit1]
	set ctt [expr $ct & 0xff ]
	set fname [format "./profiles/%1d_%07d_prof.txt" $starid $frameno]
	puts $fname
	set proftxt [open $fname "w"]

	puts $proftxt "star $starid $frameno $cx $cy $ct $profsize"
	puts $proftxt "stages $motorpos"
	puts $proftxt "xfit $xfit0 $xfit1 $xfit2 $xfit3"
	puts $proftxt "yfit $yfit0 $yfit1 $yfit2 $yfit3"

	for { set i 0 } { $i < $profsize } { incr i } { 
	    set xfit [expr $xfit3 + $xfit0 * exp(-($i-$xfit1)*($i-$xfit1)/(2*$xfit2*$xfit2) ) ]
	    set yfit [expr $yfit3 + $yfit0 * exp( - ($i-$yfit1)*($i-$yfit1)/(2*$yfit2*$yfit2))]
	    set xfit [format {%8.2f} $xfit ]
	    set yfit [format {%8.2f} $yfit ]
	    set xprof [ lindex $xprofile $i]
	    set xprof [ expr $xprof & 0xFFFF ]
	    set yprof [ lindex $yprofile $i]
	    set yprof [ expr $yprof & 0xFFFF ]
#	    puts "$i [lindex $xprofile $i] [lindex $yprofile $i] $xfit $yfit"
	    puts $proftxt "$i $xprof $yprof $xfit $yfit"
	    
	} 

	close $proftxt
    }


}
