set model [load_charts_model -tsv data/scatter.tsv -first_line_header]

sort_charts_model -model $model -column 1

echo "Base Model"

write_charts_model -model $model

set model1 [create_charts_bucket_model -model $model -column 1 -multi -delta 0.1]

echo "Bucket Model"

write_charts_model -model $model1

exit
