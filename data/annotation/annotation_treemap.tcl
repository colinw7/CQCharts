set model [load_charts_model -csv data/flare.csv -comment_header]

set plot [create_charts_plot -model $model -type empty -xmin -10 -xmax 10 -ymin -10 -ymax 10]

set maxv [get_charts_data -model $model -column 1 -name details.max]

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

  set leaf($path) 1
}

proc create_group { plot name value } {
  if {! [info exists ::groupInd($name)]} {
    set names [split $name "/"]

    if {[llength $names] > 1} {
      set lname [join [lrange $names 0 end-1] "/"]

      set pgroup [create_group $plot $lname $value]

      set ::groupInd($name) [create_charts_annotation_group -plot $plot -group $pgroup \
        -id $name -tip "$name : $value"]
    } else {
      set ::groupInd($name) [create_charts_annotation_group -plot $plot \
        -id $name -tip "$name : $value"]
    }

    set_charts_property -annotation $::groupInd($name) -name value       -value $value
    set_charts_property -annotation $::groupInd($name) -name layout.type -value TREEMAP
  }

  return $::groupInd($name)
}

proc create_rect { plot group name value } {
  set rect [create_charts_rectangle_annotation -plot $plot -group $group \
    -id $name -tip "$name : $value" -rectangle {{0 0} {10 10}}]

  set_charts_property -annotation $rect -name value -value $value

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $rect -name textInd -value $text

  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
  set_charts_property -annotation $text -name text.scaled -value 1

  set rvalue [expr {$value/$::maxv}]

  set color "palette $rvalue"

  set_charts_property -annotation $rect -name fill.color -value $color

  return $rect
}

set dr [expr {10.0/($max_depth + 1.0)}]

set da1 0.0
set da2 360.0

for {set depth 0} {$depth <= $max_depth} {incr depth} {
  set sum($depth) 0

  dict for { name value } $nameInd($depth) {
    set sum($depth) [expr {$sum($depth) + $value}]
  }

  dict for { hname value } $nameInd($depth) {
    set names [split $hname "/"]

    set rname [lindex $names end]
    set lname [join [lrange $names 0 end-1] "/"]

    if {! [info exists leaf($hname)]} {
      if {! [info exists groupInd($hname)]} {
        create_group $plot $hname $value
      }
    } else {
      if {! [info exists groupInd($lname)]} {
        create_group $plot $lname $value
      }

      set group $groupInd($lname)

      set rect [create_rect $plot $group $hname $value]
    }
  }
}

set_charts_property -annotation $group -name layout.type -value TREEMAP

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
