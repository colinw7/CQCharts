echo "Base Model"

set model [load_charts_model -csv data/birds.csv -first_line_header]

write_charts_model -model $model -max_rows 20 -max_width 14

echo "Sorted Model"

sort_charts_model -model $model -column 5 -decreasing
sort_charts_model -model $model -column 13
sort_charts_model -model $model -column 12
sort_charts_model -model $model -column 10

write_charts_model -model $model -max_rows 20 -max_width 14

echo "Folded Model (County)"

set model1 [create_charts_folded_model -model $model -column 10]

write_charts_model -model $model1 -max_rows 20 -max_width 14

echo "Folded Model (Family)"

set model2 [create_charts_folded_model -model $model1 -column 12]

write_charts_model -model $model2 -max_rows 20 -max_width 14

echo "Folded Model (Common Name)"

set model3 [create_charts_folded_model -model $model2 -column 13]

write_charts_model -model $model3 -max_width 14

exit
