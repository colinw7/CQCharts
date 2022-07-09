set model [load_charts_model -csv data/flare.csv -comment_header]

set maxv [get_charts_data -model $model -column 1 -name details.max]

set plot [create_charts_plot -model $model -type empty \
            -xmin -$maxv -xmax $maxv -ymin -$maxv -ymax $maxv]

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
    set id  $name
    set tip "$name : $value"

    set names [split $name "/"]

    if {[llength $names] > 1} {
      set lname [join [lrange $names 0 end-1] "/"]

      set pgroup [create_group $plot $lname $value]

      set ::groupInd($name) [create_charts_annotation_group -plot $plot \
                               -group $pgroup -id $id -tip $tip]
    } else {
      set ::groupInd($name) [create_charts_annotation_group -plot $plot -id $id -tip $tip]
    }

    set_charts_property -annotation $::groupInd($name) -name value       -value $value
    set_charts_property -annotation $::groupInd($name) -name layout.type -value CIRCLE
    set_charts_property -annotation $::groupInd($name) -name shapeType   -value CIRCLE

    set_charts_property -annotation $::groupInd($name) -name stroke.visible -value 1

    set_charts_property -annotation $::groupInd($name) -name state.fitted -value 1
  }

  return $::groupInd($name)
}

proc create_ellipse { plot group name value } {
  set id  $name
  set tip "$name : $value"

  set ellipse [create_charts_ellipse_annotation -plot $plot -group $group -id $id -tip $tip \
    -center {0 0} -rx $value -ry $value]

  set_charts_property -annotation $ellipse -name value -value $value

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $ellipse -name textInd -value $text

  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
  set_charts_property -annotation $text -name text.scaled -value 1

  set rvalue [expr {$value/$::maxv}]
  set color  "palette $rvalue"

  set_charts_property -annotation $ellipse -name fill.color -value $color

  set_charts_property -annotation $ellipse -name state.fitted -value 1

  return $ellipse
}

for {set depth 0} {$depth <= $max_depth} {incr depth} {
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

      set ellipse [create_ellipse $plot $group $hname $value]
    }
  }
}

set group $::groupInd(flare)

set_charts_property -annotation $group -name layout.type -value CIRCLE

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0

set rect [get_charts_property -annotation $group -name rectangle]
#echo $rect

set_charts_property -plot $plot -name range.xmin -value [lindex $rect 0]
set_charts_property -plot $plot -name range.ymin -value [lindex $rect 1]
set_charts_property -plot $plot -name range.xmax -value [lindex $rect 2]
set_charts_property -plot $plot -name range.ymax -value [lindex $rect 3]

#set view [get_charts_data -plot $plot -name view]
#set_charts_data -view $view -name fit -value 1
