set model01 [load_charts_model \
  -csv data/f1_standings_2019_01.csv -first_line_header \
  -column_type {{{Image image}}}]
set model02 [load_charts_model \
  -csv data/f1_standings_2019_02.csv -first_line_header \
  -column_type {{{Image image}}}]
set model03 [load_charts_model \
  -csv data/f1_standings_2019_03.csv -first_line_header \
  -column_type {{{Image image}}}]
set model04 [load_charts_model \
  -csv data/f1_standings_2019_04.csv -first_line_header \
  -column_type {{{Image image}}}]
set model05 [load_charts_model \
  -csv data/f1_standings_2019_05.csv -first_line_header \
  -column_type {{{Image image}}}]
set model06 [load_charts_model \
  -csv data/f1_standings_2019_06.csv -first_line_header \
  -column_type {{{Image image}}}]
set model07 [load_charts_model \
  -csv data/f1_standings_2019_07.csv -first_line_header \
  -column_type {{{Image image}}}]
set model08 [load_charts_model \
  -csv data/f1_standings_2019_08.csv -first_line_header \
  -column_type {{{Image image}}}]
set model09 [load_charts_model \
  -csv data/f1_standings_2019_09.csv -first_line_header \
  -column_type {{{Image image}}}]
set model10 [load_charts_model \
  -csv data/f1_standings_2019_10.csv -first_line_header \
  -column_type {{{Image image}}}]
set model11 [load_charts_model \
  -csv data/f1_standings_2019_11.csv -first_line_header \
  -column_type {{{Image image}}}]
set model12 [load_charts_model \
  -csv data/f1_standings_2019_12.csv -first_line_header \
  -column_type {{{Image image}}}]
set model13 [load_charts_model \
  -csv data/f1_standings_2019_13.csv -first_line_header \
  -column_type {{{Image image}}}]
set model14 [load_charts_model \
  -csv data/f1_standings_2019_14.csv -first_line_header \
  -column_type {{{Image image}}}]
set model15 [load_charts_model \
  -csv data/f1_standings_2019_15.csv -first_line_header \
  -column_type {{{Image image}}}]
set model16 [load_charts_model \
  -csv data/f1_standings_2019_16.csv -first_line_header \
  -column_type {{{Image image}}}]
set model17 [load_charts_model \
  -csv data/f1_standings_2019_17.csv -first_line_header \
  -column_type {{{Image image}}}]

set models [list $model01 $model02 $model03 $model04 $model05 $model06 $model07 $model08 \
                 $model09 $model10 $model11 $model12 $model13 $model14 $model15 $model16 \
                 $model17]

set plots {}

set i 1

foreach model $models {
  set plot [create_charts_plot -type scatter -model $model \
    -columns {{x Points} {y Pos} {label Logo} {image Image}} \
    -ymin 0 \
    -title "Formula 1 Standings (Round $i)"]

  set_charts_property -plot $plot -name visible -value 0

  set_charts_property -plot $plot -name xaxis.valueType -value INTEGER
  set_charts_property -plot $plot -name xaxis.stroke.color -value {interface 0.0}
  set_charts_property -plot $plot -name xaxis.label.text.color -value {interface 0.0}
  set_charts_property -plot $plot -name xaxis.ticks.label.text.color -value {interface 0.0}
  set_charts_property -plot $plot -name yaxis.visible -value 0

  set_charts_property -plot $plot -name title.text.color -value {interface 0.0}

  set_charts_property -plot $plot -name symbol.size -value 24px

  set_charts_property -plot $plot -name dataBox.clip -value 0
  set_charts_property -plot $plot -name dataBox.fill.color -value {interface 0.6}
  set_charts_property -plot $plot -name plotBox.fill.color -value {interface 0.6}

  set_charts_property -plot $plot -name invert.x -value 1
  set_charts_property -plot $plot -name invert.y -value 1

  set_charts_property -plot $plot -name labels.visible -value 1
  set_charts_property -plot $plot -name labels.position -value LEFT_OUTSIDE
  set_charts_property -plot $plot -name labels.text.color -value {interface 0.0}
  set_charts_property -plot $plot -name labels.text.contrast -value 0
  set_charts_property -plot $plot -name labels.text.font -value -4

  lappend plots $plot

  incr i
}

set view [get_charts_property -plot [lindex $plots 0] -name viewId]

set maxRound 17

proc show_plot { n } {
  set_charts_property -annotation $::prev_button -name fill.alpha -value 1.0
  set_charts_property -annotation $::prev_button -name text.alpha -value 1.0
  set_charts_property -annotation $::next_button -name fill.alpha -value 1.0
  set_charts_property -annotation $::next_button -name text.alpha -value 1.0

  if {$n == 1} {
    set_charts_property -annotation $::prev_button -name fill.alpha -value 0.3
    set_charts_property -annotation $::prev_button -name text.alpha -value 0.3
  }

  if {$n == $::maxRound} {
    set_charts_property -annotation $::next_button -name fill.alpha -value 0.3
    set_charts_property -annotation $::next_button -name text.alpha -value 0.3
  }

  set i 1

  foreach plot $::plots {
    set visible [expr {$i == $n}]

    set_charts_property -plot $plot -name visible -value $visible

    incr i
  }

  set plot  [lindex $::plots  [expr {$n - 1}]]
  set model [lindex $::models [expr {$n - 1}]]

  set loc  [get_charts_data -model $model -name meta -data location]
  set year [get_charts_data -model $model -name meta -data year]

  set_charts_property -plot $plot -name title.text.string \
    -value "Formula 1 Standings (Round $n) $loc $year"
}

set round 17

proc annotationSlot { viewId id } {
  if       {$id == "next"} {
    if {$::round < $::maxRound} {
      incr ::round 1
    }
  } elseif {$id == "prev"} {
    if {$::round > 1} {
      incr ::round -1
    }
  } elseif {$id == "first"} {
    set ::round 1
  } elseif {$id == "last"} {
    set ::round $::maxRound
  }

  show_plot $::round
}

set tx 32
set ty 32

set first_button [create_charts_text_annotation -view $view -id first \
  -position [list ${tx} ${ty} px] -text "First" -stroked 1 -filled 1]

incr tx 80

set prev_button [create_charts_text_annotation -view $view -id prev \
  -position [list ${tx} ${ty} px] -text "Prev" -stroked 1 -filled 1]

incr tx 80

set next_button [create_charts_text_annotation -view $view -id next \
  -position [list ${tx} ${ty} px] -text "Next" -stroked 1 -filled 1]

incr tx 80

set last_button [create_charts_text_annotation -view $view -id last \
  -position [list ${tx} ${ty} px] -text "Last" -stroked 1 -filled 1]

connect_charts_signal -view $view -from annotationIdPressed -to annotationSlot

show_plot $round
