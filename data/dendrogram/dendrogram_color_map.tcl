set model [load_charts_model -csv data/dendogram_color_map.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{link 0} {value 1} {color 3}}]

set_charts_property -plot $plot -name root.size -value 0.1P
set_charts_property -plot $plot -name hier.size -value 0.1P
set_charts_property -plot $plot -name leaf.size -value 0.1P

set_charts_property -plot $plot -name mapping.color.color_map \
  -value {{Stage1 red} {Stage2 green} {Stage3 blue}}
