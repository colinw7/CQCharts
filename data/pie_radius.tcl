set modelId [load_charts_model -csv data/aster_data.csv -first_line_header]

set_charts_data -model $modelId -name column_type -column 4 -value {{color}}

set plotId [create_charts_plot -type pie \
  -columns {{id 0} {radius 2} {values 3} {label 5} {color 4}} -title "Aster"]

set viewId [get_charts_property -plot $plotId -name viewId]

set_charts_property -plot $plotId -name options.donut       -value 1
set_charts_property -plot $plotId -name grid.visible        -value 1
set_charts_property -plot $plotId -name options.innerRadius -value 0.2
