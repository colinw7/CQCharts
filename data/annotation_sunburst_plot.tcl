set model [load_charts_model -csv data/flare.csv -comment_header]

set plot [create_charts_plot -model $model -type empty -xmin -10 -xmax 10 -ymin -10 -ymax 10]

set nr [get_charts_data -model $model -name num_rows   ]
set nc [get_charts_data -model $model -name num_columns]

set max_depth 0

for {set r 0} {$r < $nr} {incr r} {
  set path  [get_charts_data -model $model -column 0 -row $r -name value -role display]
  set value [get_charts_data -model $model -column 1 -row $r -name value -role display]

  set names [split $path "/"]

  set depth 0

  set hname ""

  foreach name $names {
    if {$hname == ""} {
      set hname $name
    } else {
      set hname "$hname/$name"
    }

    if {$depth > $max_depth} {
      set max_depth $depth
    }

    if {! [info exists nameInd($depth)]} {
      set nameInd($depth) {}
    }

    if {! [dict exists $nameInd($depth) $hname]} {
      dict set nameInd($depth) $hname $value
      incr ind
    } else {
      set nvalue [dict get $nameInd($depth) $hname]

      dict set nameInd($depth) $hname [expr {$nvalue + $value}]
    }

    incr depth
  }
}

proc create_arc { plot r1 r2 a da name path value svalue } {
  #echo "create_arc $plot $r1 $r2 $a $da $name $value"

  set id  $path
  set tip "$path : $value"

  set arc [create_charts_pie_slice_annotation -plot $plot -position {0 0} -id $id -tip $tip \
   -inner_radius $r1 -outer_radius $r2 -start_angle $a -span_angle $da]

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $arc  -name textInd -value $text
  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
# set_charts_property -annotation $text -name text.scaled -value 1
  set_charts_property -annotation $text -name text.font -value -8

  set ta [expr {$a + $da/2.0}]

  if {$ta > 90.0 && $ta < 270.0} {
    set ta [expr {$ta - 180.0}]
  }

  set_charts_property -annotation $text -name text.angle -value $ta

  set_charts_property -annotation $arc  -name state.editable -value 0
  set_charts_property -annotation $text -name state.editable -value 0

  set color "palette $svalue"

  set_charts_property -annotation $arc -name fill.color -value $color

  return $arc
}

set dr [expr {10.0/($max_depth + 1.0)}]

set da1 0.0
set da2 360.0

for {set depth 0} {$depth <= $max_depth} {incr depth} {
  set sum($depth) 0

  dict for { name value } $nameInd($depth) {
    set sum($depth) [expr {$sum($depth) + $value}]
  }

  set r1 [expr {$depth*$dr}]
  set r2 [expr {$r1 + $dr}]

  set a -1

  dict for { hname value } $nameInd($depth) {
    set names [split $hname "/"]

    set rname [lindex $names end]
    set lname [join [lrange $names 0 end-1] "/"]

    if {$lname == ""} {
      if {$a < 0} {
        set a 0
      }

      set svalue [expr {1.0*$value/$sum($depth)}]
      set da     [expr {$svalue*360.0}]
    } else {
      set pvalue [dict get $arcData($lname) value]
      set pa     [dict get $arcData($lname) a]
      set pda    [dict get $arcData($lname) span]

      #echo "Parent: $lname $pvalue $pa $pda"

      set svalue [expr {1.0*$value/$pvalue}]
      set a      $pa
      set da     [expr {$svalue*$pda}]
    }

    #echo "Arc: $hname $value $a $da"

    dict set arcData($hname) value $value
    dict set arcData($hname) angle $a
    dict set arcData($hname) span  $da
    dict set arcData($hname) a     $a

    set arc [create_arc $plot $r1 $r2 $a $da $rname $hname $value $svalue]

    set a [expr {$a + $da}]

    if {$lname != ""} {
      dict set arcData($lname) a $a
    }
  }
}

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
