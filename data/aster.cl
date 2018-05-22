load_model -csv data/aster_data.csv -first_line_header
modelId = _rc

get_property -model $modelId -name num_rows
nr = _rc

total = 0.0

for (i = 0; i < nr; ++i)
  get_model -ind $modelId -row $i -column 2 -name value
  score = _rc

  get_model -ind $modelId -row $i -column 3 -name value
  weight = _rc

  total += score*weight
endfor

average = total / nr

set_model -ind $modelId -column_type "4#color"

create_plot -type pie -columns "id=0,radius=2,data=3,label=5,color=4" -title "Aster"
plotId = _rc

get_property -plot $plotId -name viewId
viewId = _rc

set_property -view $viewId -plot $plotId -name donut -value 1
set_property -view $viewId -plot $plotId -name grid.visible -value 1
set_property -view $viewId -plot $plotId -name innerRadius -value 0.2

text_shape -plot $plotId -x 0 -y 0 -text $average -align center
textId = _rc
