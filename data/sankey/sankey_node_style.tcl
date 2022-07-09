# Sankey From/To
set model [load_charts_model -csv data/sankey_style.csv -first_line_header]

set plot [create_charts_plot -model $model -type sankey \
  -columns {{from 0} {to 1} {attributes 2}} -title "sankey style"]

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0
