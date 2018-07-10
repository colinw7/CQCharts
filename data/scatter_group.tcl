proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_model -tsv data/scatter.tsv -first_line_header]

set plot [create_plot -model $model -type scatter \
  -columns "x=petalLength,y=sepalLength,group=species" \
  -properties "xaxis.userLabel=Petal Length,yaxis.userLabel=Sepal Length"]

connect_chart -plot $plot -from objIdPressed -to objPressed
