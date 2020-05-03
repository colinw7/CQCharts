set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

#set model1 [create_charts_folded_model -model $model -column species -fold_data]
#set model2 [create_charts_folded_model -model $model1 -column sepalLength -fold_data]
#set model3 [create_charts_collapse_model -model $model2 -sum "1 2 3"]

set model1 [create_charts_folded_model -model $model -column sepalLength -fold_data]
set model3 [create_charts_collapse_model -model $model1 -sum "1 2 3"]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model3 -type barchart -columns {{name sepalLength} {value Count}}]
