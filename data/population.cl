proc annotationSlot(viewId, id)
  print viewId, id

  #get_property -view $viewId -annotation $id -name position
  #get_property -view $viewId -annotation $id -name text

  if     (id == "next")
    if (_year < _maxYear)
      _year += 10
    endif
  elseif (id == "prev")
    if (_year > _minYear)
      _year -= 10
    endif
  elseif (id == "overlay")
    _place = id
  elseif (id == "sidebyside")
    _place = id
  endif

  plotYear(_year)
endproc

proc plotYear(year)
  if (_viewId != "")
    remove_plot -view $_viewId -all
  endif

  filter = "sex:1,year:%d" % year
  title  = "Male %d" % year

  load -csv data/population.csv -first_line_header -filter $filter -title $title
  model1Id = _rc

  add_plot -type barchart -columns "name=1,value=3" -title $title
  _plot1Id = _rc

  set_property -view $_viewId -plot $_plot1Id -name fill.color -value "#4444aa"
  set_property -view $_viewId -plot $_plot1Id -name fill.alpha -value 0.5
  set_property -view $_viewId -plot $_plot1Id -name key.visible -value 0

  filter = "sex:2,year:%d" % year
  title  = "Female %d" % year

  load -csv data/population.csv -first_line_header -filter $filter -title $title
  model2Id = _rc

  add_plot -type barchart -columns "name=1,value=3" -title $title
  _plot2Id = _rc

  set_property -view $_viewId -plot $_plot2Id -name fill.color -value "#aa4444"
  set_property -view $_viewId -plot $_plot2Id -name fill.alpha -value 0.5
  set_property -view $_viewId -plot $_plot2Id -name key.visible -value 0

  if     (_place == "sidebyside")
    place -horizontal $_plot1Id $_plot2Id
  elseif (_place == "overlay")
    group -overlay $_plot1Id $_plot2Id
  endif
endproc

load -csv data/population.csv -first_line_header
modelId = _rc
print modelId

get_model -ind $modelId -name num_rows
print _rc
get_model -ind $modelId -name num_columns
print _rc

get_model -ind $modelId -column 0 -name min
_minYear = _rc
get_model -ind $modelId -column 0 -name max
_maxYear = _rc

_year = _minYear

_viewId = ""

_place = "overlay"

plotYear(_year)

get_view -current
_viewId = _rc

text_shape -id prev -x 10 -y 95 -text "Prev" -border 1 -background 1
text1Id = _rc

text_shape -id next -x 20 -y 95 -text "Next" -border 1 -background 1
text2Id = _rc

text_shape -id overlay -x 30 -y 95 -text "Overlay" -border 1 -background 1
text3Id = _rc

text_shape -id sidebyside -x 40 -y 95 -text "Side By Side" -border 1 -background 1
text4Id = _rc
connect -view $_viewId -from annotationIdPressed -to annotationSlot
