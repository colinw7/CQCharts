echo "Base Model"

set model [load_charts_model -csv data/birds.csv -first_line_header]

write_charts_model -model $model -max_rows 10

sort_charts_model -model $model -column 4
sort_charts_model -model $model -column 0

echo "Folded Model (String)"

set model1 [create_charts_folded_model -model $model -column 4 -fold_data]

write_charts_model -model $model1 -max_rows 10

echo "Folded Model (Real)"

set model2 [create_charts_folded_model -model $model -column 0 -fold_data -bucket_delta 0.2]

write_charts_model -model $model2 -max_rows 10

echo "Folded Folded Model"

set model3 [create_charts_folded_model -model $model1 -column 1 -fold_data -bucket_delta 0.2]

write_charts_model -model $model3 -max_rows 10
