set expr_filter     {$sex == 1 && $year == 1860}
set regexp_filter   "year:186.*"
set wildcard_filter "year:186*"
set simple_filter   "sex:1,year:1860"

set model1 [load_charts_model -csv data/population.csv -first_line_header \
  -filter $expr_filter -filter_type expression]

set model2 [load_charts_model -csv data/population.csv -first_line_header \
  -filter $regexp_filter -filter_type regexp]

set model3 [load_charts_model -csv data/population.csv -first_line_header \
  -filter $wildcard_filter -filter_type wildcard]

set model4 [load_charts_model -csv data/population.csv -first_line_header \
  -filter $simple_filter -filter_type simple]

show_charts_manage_models_dlg
