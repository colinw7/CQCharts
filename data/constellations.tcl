echo "Load Star Positions"
set model1 [load_charts_model -csv big_data/hygdata_v3.csv -first_line_header]

echo "Load Constellations Lines"
set model2 [load_charts_model -csv big_data/constellationship.csv]

echo "Load Constellation Names"
set model3 [load_charts_model -csv big_data/constellation_names.eng.csv]

#---

echo "Map Constellations to Star Positions"
set nr [get_charts_data -model $model2 -name num_rows]

set polys {}
set names {}

for {set r 0} {$r < $nr} {incr r} {
  set name [get_charts_data -model $model2 -name value -row $r -column 0]

  set nrow [get_charts_data -model $model3 -name find -column 0 -data $name]

  set fullname [get_charts_data -model $model3 -name value -row $nrow -column 1]

  #if {$name != "UMa"} { continue }

  set stars [get_charts_data -model $model2 -name value -row $r -column 2]

  set poly {}

  set ind 0

  foreach star $stars {
    set row [get_charts_data -model $model1 -name find -column 1 -data $star]

    if {$ind == 0} {
      set x1 [get_charts_data -model $model1 -name value -row $row -column ra]
      set y1 [get_charts_data -model $model1 -name value -row $row -column dec]

      set left  0
      set right 0

      if       {$x1 < 6} {
        set left 1
      } elseif {$x1 > 18} {
        set right 1
      }

      lappend poly [list $x1 $y1]
    } else {
      set x2 [get_charts_data -model $model1 -name value -row $row -column ra]
      set y2 [get_charts_data -model $model1 -name value -row $row -column dec]

      if       {$left == 1} {
        if {$x2 > 18} {
          set x2 [expr {$x2 - 24}]
        }
      } elseif {$right == 1} {
        if {$x2 < 6} {
          set x2 [expr {$x2 + 24}]
        }
      }

      lappend poly [list $x2 $y2]
    }

    incr ind
  }

  lappend names $fullname
  lappend polys $poly
}

#---

echo "Filter Stars"
filter_charts_model -model $model1 -expr {$mag <= 6.5}

#---

echo "Create Plot"
set plot [create_charts_plot -model $model1 -type scatter \
  -columns {{x ra} {y dec} {name hip} {color absmag}}]

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name theme.dark -value 1
set_charts_property -view $view -name inside.highlight.stroke.color -value red

set_charts_property -plot $plot -name key.visible -value 0
set_charts_property -plot $plot -name points.stroke.visible -value 0
set_charts_property -plot $plot -name points.fill.alpha -value 0.4
set_charts_property -plot $plot -name points.size -value 3px

set_charts_property -plot $plot -name coloring.defaultPalette -value moreland
set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0
set_charts_property -plot $plot -name dataBox.fill.visible -value 0

set_charts_property -plot $plot -name points.scaled -value 0

#---

echo "Add Annotations"
foreach poly $polys name $names {
  set ann [create_charts_polyline_annotation -plot $plot -points $poly -id $name]

  set_charts_property -annotation $ann -name pointsType -value LINE_PAIRS
  set_charts_property -annotation $ann -name stroke.width -value 3px

  set text [create_charts_text_annotation -plot $plot -position {0 0} -text $name]

  set_charts_property -annotation $text -name text.font -value -6
  set_charts_property -annotation $text -name text.align -value {AlignHCenter|AlignVCenter}
  set_charts_property -annotation $text -name text.color -value #c3c300
  set_charts_property -annotation $text -name text.contrast -value 1

  set_charts_property -annotation $ann -name textInd -value $text
}
