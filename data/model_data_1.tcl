set model [load_charts_model -csv data/diamonds.csv -first_line_header]

# headers
set headers [get_charts_data -model $model -name header]
echo "headers: $headers"

set header [get_charts_data -model $model -name header -column 1]
echo "header 1: $header"

# dim
set nr [get_charts_data -model $model -name num_rows]
set nc [get_charts_data -model $model -name num_columns]

echo "dims: $nr $nc"

# summary
set sds [get_charts_data -model $model -name standard_deviation]
echo "standard_deviations: $sds"

set sd [get_charts_data -model $model -name standard_deviation -column 4]
echo "standard_deviation 4: $sd"

# summary model
set model1 [create_charts_summary_model -model $model -sorted]

set order [get_charts_data -model $model1 -name property.sortOrder]

echo "order: $order"

write_charts_model -model $model1 -max_rows 10

set_charts_data -model $model1 -name property.sortOrder -value 1

set order [get_charts_data -model $model1 -name property.sortOrder]

echo "order: $order"

write_charts_model -model $model1 -max_rows 10
