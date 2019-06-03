set modelId [load_charts_model -csv data/sea_level.csv -first_line_header]

set_charts_data -model $modelId -name column_type -column 0 -value {{real} {format %gM}}

set plotId [create_charts_plot -type bar -columns {{group 1} {value 0} {color 2} {label 3}} \
  -ymax 8000]

set_charts_property -plot $plotId -name labels.visible -value 1
set_charts_property -plot $plotId -name labels.position -value TOP_OUTSIDE
set_charts_property -plot $plotId -name invert.y -value 1
set_charts_property -plot $plotId -name yaxis.grid.lines -value MAJOR
