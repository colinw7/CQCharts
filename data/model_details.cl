#load_model -tsv data/digits.tsv -first_line_header
#load_model -csv data/aster_data.csv -first_line_header
#load_model -csv data/cities.csv -first_line_header -column_type "3#real:min=0,max=1;4#color"
load_model -tsv data/multi_series.tsv -comment_header -column_type "0#time:format=%Y%m%d"
#load_model -tsv data/cities1.dat -comment_header -column_type "2#real:min=0"
modelId = _rc

print modelId

get_property -model $modelId -name num_rows
nr = _rc

get_property -model $modelId -name num_columns
nc = _rc

print nr,nc

for (c = 0; c < nc; ++c)
  print "Column", c

  get_model -ind $modelId -column $c -name type 
  type = _rc
  get_model -ind $modelId -column $c -name min
  minv = _rc
  get_model -ind $modelId -column $c -name max
  maxv = _rc

  print "  Type",type
  print "  Range",minv,maxv

  get_model -ind $modelId -column $c -name monotonic 
  monotonic = _rc

  get_model -ind $modelId -column $c -name increasing
  increasing = _rc

  print "  Monotonic",monotonic
  print "  Increasing",increasing

  get_model -ind $modelId -column $c -name num_unique
  num_unique = _rc

  print "  Num Unique",num_unique
endfor

if (nr > 100)
  nr = 100
endif

for (r = 0; r < nr; ++r)
  for (c = 0; c < nc; ++c)
    get_model -ind $modelId -column $c -row $r -name value
    value = _rc

    get_model -ind $modelId -column $c -row $r -name map
    map = _rc

    print "  Map",r,c,value,map
  endfor
endfor
