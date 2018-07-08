proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_plot -model $model -type scatter \
  -columns "x=sepalLength,y=sepalWidth" \
  -properties "symbol.type=circle,symbol.size=5px" \
  -properties "xaxis.userLabel=Sepal Length,yaxis.userLabel=Sepal Width"]

connect_chart -plot $plot1 -from objIdPressed -to objPressed

set plot2 [create_plot -model $model -type box \
  -columns "value=sepalLength,group=species" \
  -properties "yaxis.userLabel=Sepal Length"]

connect_chart -plot $plot2 -from objIdPressed -to objPressed

place_plots -horizontal $plot1 $plot2
