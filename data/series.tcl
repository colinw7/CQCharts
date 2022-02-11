set modelId [load_charts_model -csv data/series.csv -first_line_header]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Series"]

set_charts_property -plot $plot -name fillUnder.visible -value 1
set_charts_property -plot $plot -name horizon.layers -value 2
