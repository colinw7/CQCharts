# Dendrogram

set model [load_charts_model -csv data/flare.csv -comment_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{name 0} {value 1} {size 1} {color 1}} -title "Circular Dendrogram"]

set_charts_property -plot $plot -name options.placeType -value CIRCULAR

set_charts_property -plot $plot -name hier.label.font -value {-8}
set_charts_property -plot $plot -name leaf.label.font -value {-12}
set_charts_property -plot $plot -name hier.label.rotated -value {true}

set_charts_property -plot $plot -name points.scaled -value {false}

set_charts_property -plot $plot -name scaling.equal -value {true}

connect_charts_signal -plot $plot -from plotObjsAdded -to init_plot

set inited 0

proc init_plot { view plot } {
  if {$::inited == 0} {
    set ::inited 1

    execute_charts_slot -plot $plot -name expand_all
  }
}
