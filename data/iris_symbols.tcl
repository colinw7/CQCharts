create_charts_symbol_set -name iris

add_charts_symbol -set iris -svg data/setosa.svg     -styled 0 -name setosa_shape -filled 1
add_charts_symbol -set iris -svg data/versicolor.svg -styled 0 -name versicolor_shape -filled 1
add_charts_symbol -set iris -svg data/virginica.svg  -styled 0 -name virginica_shape -filled 1
add_charts_symbol -set iris -svg data/setosa.svg     -styled 1 -name setosa_style -filled 1
add_charts_symbol -set iris -svg data/versicolor.svg -styled 1 -name versicolor_style -filled 1
add_charts_symbol -set iris -svg data/virginica.svg  -styled 1 -name virginica_style -filled 1

set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x petalLength} {y sepalLength} {group species}} \
  -properties {{xaxis.userLabel {Petal Length}} {yaxis.userLabel {Sepal Length}}} \
  -title "Iris Petal Shapes"]
