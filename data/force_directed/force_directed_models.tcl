set model1 [load_charts_model -csv data/force_directed_model1.csv -first_line_header]
set model2 [load_charts_model -csv data/force_directed_model2.csv -first_line_header]

set plot [create_charts_plot -model $model1 -type force_directed \
  -columns {{from 0} {to 1}}]

set_charts_data -plot $plot -name extra_model -value $model2

set_charts_property -plot $plot -name nodeModel.model -value $model2

set_charts_property -plot $plot -name nodeModel.idColumn      -value 0
set_charts_property -plot $plot -name nodeModel.colorColumn   -value 1
set_charts_property -plot $plot -name nodeModel.initPosColumn -value 2
