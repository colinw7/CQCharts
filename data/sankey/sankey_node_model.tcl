# Sankey From/To
set model1 [load_charts_model -csv data/sankey_model1.csv -first_line_header]
set model2 [load_charts_model -csv data/sankey_model2.csv -first_line_header]

set plot [create_charts_plot -model $model1 -type sankey \
  -columns {{from 0} {to 1}} -title "sankey style"]

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0

set_charts_data -plot $plot -name extra_model -value $model2

set_charts_property -plot $plot -name nodeModel.model -value $model2

set_charts_property -plot $plot -name nodeModel.idColumn          -value 0
set_charts_property -plot $plot -name nodeModel.fillColorColumn   -value 1
set_charts_property -plot $plot -name nodeModel.fillAlphaColumn   -value 2
set_charts_property -plot $plot -name nodeModel.fillPatternColumn -value 3
set_charts_property -plot $plot -name nodeModel.strokeWidthColumn -value 4
set_charts_property -plot $plot -name nodeModel.strokeDashColumn  -value 5
set_charts_property -plot $plot -name nodeModel.strokeColorColumn -value 6
set_charts_property -plot $plot -name nodeModel.strokeAlphaColumn -value 7
