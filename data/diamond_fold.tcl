set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set model1 [create_charts_folded_model -model $model -column cut]

set model2 [create_charts_collapse_model -model $model1]

write_charts_model -model $model1
write_charts_model -model $model2
