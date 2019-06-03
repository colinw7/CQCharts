proc objPressed { view plot id } {
  echo "$view $plot $id"

  set inds [get_charts_data -plot $plot -object $id -name inds]

  echo "$inds"
}

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set groups [get_charts_data -model $model -column species -name unique_values]

echo $groups

set plots {}

set dc [expr {1.0/[llength $groups]}]

set c 0.0

foreach group $groups {
  set plot [create_charts_plot -model $model -type scatter \
    -columns {{x petalLength} {y sepalLength}} \
    -where "@{species}=={$group}" \
    -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}} \
    -properties [list [list symbol.fill.color [list palette $c]]] \
    -title "$group"]

  connect_charts_signal -plot $plot -from objIdPressed -to objPressed

  lappend plots $plot

  set c [expr {$c + $dc}]
}

place_charts_plots -horizontal $plots
