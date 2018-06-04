proc annotationSlot(viewId, id)
  #print viewId, id

  #get_property -annotation $id -name position
  #get_property -annotation $id -name text

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

  load_model -csv data/population.csv -first_line_header -filter $filter
  model1Id = _rc

  title = "Male %d" % year

  create_plot -type barchart -columns "name=1,value=3" -title $title
  _plot1Id = _rc

  set_property -plot $_plot1Id -name fill.color -value "#4444aa"
  set_property -plot $_plot1Id -name fill.alpha -value 0.5
  set_property -plot $_plot1Id -name key.visible -value 0

  filter = "sex:2,year:%d" % year

  load_model -csv data/population.csv -first_line_header -filter $filter
  model2Id = _rc

  title = "Female %d" % year

  create_plot -type barchart -columns "name=1,value=3" -title $title
  _plot2Id = _rc

  set_property -plot $_plot2Id -name fill.color -value "#aa4444"
  set_property -plot $_plot2Id -name fill.alpha -value 0.5
  set_property -plot $_plot2Id -name key.visible -value 0

  if     (_place == "sidebyside")
    place_plots -horizontal $_plot1Id $_plot2Id
  elseif (_place == "overlay")
    group_plots -overlay $_plot1Id $_plot2Id
  endif
endproc

load_model -csv data/population.csv -first_line_header
modelId = _rc
#print modelId

get_charts_data -model $modelId -name num_rows
#print _rc
get_charts_data -model $modelId -name num_columns
#print _rc

get_charts_data -model $modelId -column 0 -name min
_minYear = _rc
get_charts_data -model $modelId -column 0 -name max
_maxYear = _rc

_year = _minYear

_viewId = ""

_place = "overlay"

plotYear(_year)

get_property -plot $_plot1Id -name viewId
_viewId = _rc

measure_text -view $_viewId -name width -data "xx"
dtx = _rc
print dtx

tx = 2*dtx
print tx

create_text_shape -id prev -x $tx -y 95 -text "Prev" -border 1 -background 1
text1Id = _rc

measure_text -view $_viewId -name width -data "Prev"
tx = tx + _rc + dtx
print tx

create_text_shape -id next -x $tx -y 95 -text "Next" -border 1 -background 1
text2Id = _rc

measure_text -view $_viewId -name width -data "Next"
tx = tx + _rc + 2*dtx
print tx

create_text_shape -id overlay -x $tx -y 95 -text "Overlay" -border 1 -background 1
text3Id = _rc

measure_text -view $_viewId -name width -data "Overlay"
tx = tx + _rc + dtx
print tx

create_text_shape -id sidebyside -x $tx -y 95 -text "Side By Side" -border 1 -background 1
text4Id = _rc
connect_chart -view $_viewId -from annotationIdPressed -to annotationSlot
