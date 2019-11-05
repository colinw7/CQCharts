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
set model18 [load_charts_model \
  -csv data/f1_standings_2019_18.csv -first_line_header \
  -column_type {{{Image image}}}]
set model19 [load_charts_model \
  -csv data/f1_standings_2019_19.csv -first_line_header \
  -column_type {{{Image image}}}]

set models [list $model01 $model02 $model03 $model04 $model05 $model06 $model07 $model08 \
                 $model09 $model10 $model11 $model12 $model13 $model14 $model15 $model16 \
                 $model17 $model18 $model19]

set splots {}
set tplots {}

set i 1

foreach model $models {
  set splot [create_charts_plot -type scatter -model $model \
    -columns {{x Points} {y Pos} {label Logo} {image Image} {tips {Driver}}} \
    -ymin 0 \
    -title "Formula 1 Standings (Round $i)"]

  set_charts_property -plot $splot -name range.view -value {0 33.3 100 100}

  set_charts_property -plot $splot -name visible -value 0

  set_charts_property -plot $splot -name xaxis.valueType -value INTEGER
  set_charts_property -plot $splot -name xaxis.stroke.color -value {interface 0.0}
  set_charts_property -plot $splot -name xaxis.label.text.color -value {interface 0.0}
  set_charts_property -plot $splot -name xaxis.ticks.label.text.color -value {interface 0.0}
  set_charts_property -plot $splot -name yaxis.visible -value 0

  set_charts_property -plot $splot -name title.text.color -value {interface 0.0}

  set_charts_property -plot $splot -name symbol.size -value 24px

  set_charts_property -plot $splot -name dataBox.clip -value 0
  set_charts_property -plot $splot -name dataBox.fill.color -value {interface 0.6}
  set_charts_property -plot $splot -name plotBox.fill.color -value {interface 0.6}

  set_charts_property -plot $splot -name invert.x -value 1
  set_charts_property -plot $splot -name invert.y -value 1

  set_charts_property -plot $splot -name labels.visible -value 1
  set_charts_property -plot $splot -name labels.position -value LEFT_OUTSIDE
  set_charts_property -plot $splot -name labels.text.color -value {interface 0.0}
  set_charts_property -plot $splot -name labels.text.contrast -value 0
  set_charts_property -plot $splot -name labels.text.font -value -4

  lappend splots $splot

  #---

  set tplot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3 4 5 6 7 8}}}]

  set_charts_property -plot $tplot -name range.view -value {0 0 100 33.3}

  set_charts_property -plot $tplot -name visible -value 0

  lappend tplots $tplot

  #---

  incr i
}

set view [get_charts_property -plot [lindex $splots 0] -name viewId]

set_charts_property -view $view -name sizing.auto -value 0

set maxRound 19

proc show_plot { n } {
  set_charts_property -annotation $::prev_button -name enabled -value 1

  set_charts_property -annotation $::next_button -name enabled -value 1

  if {$n == 1} {
    set_charts_property -annotation $::prev_button -name enabled -value 0
  }

  if {$n == $::maxRound} {
    set_charts_property -annotation $::next_button -name enabled -value 0
  }

  set np [llength $::splots]

  for {set i 1} {$i <= $np} {incr i} {
    set visible [expr {$i == $n}]

    set i1 [expr {$i - 1}]

    set_charts_property -plot [lindex $::splots $i1] -name visible -value $visible
    set_charts_property -plot [lindex $::tplots $i1] -name visible -value $visible
  }

  set splot [lindex $::splots [expr {$n - 1}]]
  set tplot [lindex $::tplots [expr {$n - 1}]]
  set model [lindex $::models [expr {$n - 1}]]

  set loc  [get_charts_data -model $model -name meta -data location]
  set year [get_charts_data -model $model -name meta -data year]

  set_charts_property -plot $splot -name title.text.string \
    -value "Formula 1 Standings (Round $n) - $loc ($year)"
}

set round 19

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

set first_button [create_charts_button_annotation -view $view -id first \
  -position [list ${tx} ${ty} px] -text "First"]

incr tx 80

set prev_button [create_charts_button_annotation -view $view -id prev \
  -position [list ${tx} ${ty} px] -text "Prev"]

incr tx 80

set next_button [create_charts_button_annotation -view $view -id next \
  -position [list ${tx} ${ty} px] -text "Next"]

incr tx 80

set last_button [create_charts_button_annotation -view $view -id last \
  -position [list ${tx} ${ty} px] -text "Last"]

connect_charts_signal -view $view -from annotationIdPressed -to annotationSlot

for {set i 1} {$i <= 19} {incr i} {
  show_plot $i
  qt_sync
}

set_charts_property -view $view -name sizing.fixedSize -value {1600 2000}
qt_sync

define_charts_proc -svg annotationClick id {
  if (typeof window.currentPlot == 'undefined') {
    window.currentPlot = 1;
  }

  var plots = [];

  for (var i = 1; i <= 19; ++i) {
    plots.push(document.getElementById("plot_scatter" + i.toString()));
    plots.push(document.getElementById("plot_table"   + i.toString()));
  }

  if      (id == 'first') {
    currentPlot = 1;
  }
  else if (id == 'last') {
    currentPlot = 19;
  }
  else if (id == 'prev') {
    currentPlot = currentPlot - 1;

    if (currentPlot < 1) {
      currentPlot = 1;
    }
  }
  else if (id == 'next') {
    currentPlot = currentPlot + 1;

    if (currentPlot > 19) {
      currentPlot = 19;
    }
  }

  for (var i = 1; i <= 19; ++i) {
    var i1 = (i - 1)*2 + 1;

    plots[i1 - 1].style.visibility=(currentPlot == i ? "visible" : "hidden");
    plots[i1    ].style.visibility=(currentPlot == i ? "visible" : "hidden");
  }

  var btn = document.getElementById("prev"); btn.disabled = (currentPlot == 1);
  var btn = document.getElementById("next"); btn.disabled = (currentPlot == 19);
}

define_charts_proc -script annotationClick id {
  if (typeof window.currentPlot == 'undefined') {
    window.currentPlot = 1;
  }

  if      (id == 'first') {
    currentPlot = 1;
  }
  else if (id == 'last') {
    currentPlot = 19;
  }
  else if (id == 'prev') {
    currentPlot = currentPlot - 1;

    if (currentPlot < 1) {
      currentPlot = 1;
    }
  }
  else if (id == 'next') {
    currentPlot = currentPlot + 1;

    if (currentPlot > 19) {
      currentPlot = 19;
    }
  }

  for (var i = 1; i <= 19; ++i) {
    var i1 = (i - 1)*2 + 1;

    charts.plots[i1 - 1].visible = (currentPlot == i);
    charts.plots[i1    ].visible = (currentPlot == i);
  }

  var btn = document.getElementById("prev"); btn.disabled = (currentPlot == 1);
  var btn = document.getElementById("next"); btn.disabled = (currentPlot == 19);
}

show_plot $round
