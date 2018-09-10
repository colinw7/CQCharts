# Command Summary #
 
 + load, process, sort, filter model
   + `load_model`
   + `process_model`
   + `add_process_model_proc`
   + `sort_model`
   + `fold_model`
   + `filter_model`
   + `flatten_model`

 + correlation, subset
   + `correlation_model`
   + `subset_model`
   + `transpose_model`

 + export
   + `export_model`

 + measure text
   + `measure_charts_text`

 + add view
   + `create_view`

 + add/remove plot
   + `create_plot`
   + `remove_plot`

 + group/place plots
   + `group_plots`
   + `place_plots`

 + get/set charts property
   + `get_charts_property`
   + `set_charts_property`

 + get/set charts model data
   + `get_charts_data`
   + `set_charts_data`

 + annotations
   + `create_text_shape`
   + `create_arrow_shape`
   + `create_rect_shape`
   + `create_ellipse_shape`
   + `create_polygon_shape`
   + `create_polyline_shape`
   + `create_point_shape`
   + `remove_shape`

 + theme/palette
   + `get_palette`
   + `set_palette`
   + `connect_chart`

 + print
   + `print_chart`

 + dialogs
   + `load_model_dlg`
   + `manage_model_dlg`
   + `create_plot_dlg`

# Command Details #

## Load Model ##

```
load_model
  -csv |-tsv |-json |-data |-expr |-var <variable name>
  [-comment_header ]
  [-first_line_header ]
  [-first_column_header ]
  [-separator <>]
  [-transpose ]
  [-num_rows <number of rows>]
  [-filter <filter expression>]
  [-column_type <column type>]
  [-name <name>]
  [<file name>]
  [-help]
```

Example:
```
set model [load_model -csv data.data -first_line_header]
```

## Create Plot from Model ##

```
create_plot
  -view <view_id>
  -model <model_ind>
  -type <type>
  [-where <filter>]
  [-columns <columns>]
  [-bool <name_values>]
  [-string <name_values>]
  [-real <name_values>]
  [-xintegral <>]
  [-yintegral <>]
  [-xlog <>]
  [-ylog <>]
  [-title <title>]
  [-properties <name_values>]
  [-position <position box>]
  [-xmin <x>]
  [-ymin <y>]
  [-xmax <x>]
  [-ymax <y>]
  [-help]
```

Example:
```
set plot [create_plot -model $model -type xy -columns "x=0,y=1"]
```

## Set Plot Properties ##

```
set_charts_property
  -view <view name>|-plot <plot name>|-annotation <annotation name>
  [-name <property name>]
  [-value <property view>]
  [-help]
```

## Modify Model Data ##

```
process_model
  -model <model index>
  [-column <column for delete, modify, calc, query>]
  -add |-delete |-modify |-calc |-query |-list 
  [-header <header label for add/modify>]
  [-type <type data for add/modify>]
  [-expr <expression for add/modify/calc/query>]
  [-help]
```
