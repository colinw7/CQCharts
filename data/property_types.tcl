set names [get_charts_data -name property_type_names]

echo "Property Types [llength $names]: $names"
echo "---"

set names [get_charts_data -name meta_type_names]

echo "Meta Types [llength $names]: $names"
echo "---"

proc test_prop_data { name data } {
  set value1 [get_charts_data -name property_type_data -data $name]
  set_charts_data -name property_type_data -data $name -value $data
  set value2 [get_charts_data -name property_type_data -data $name]
  echo "Set $data: $name $value2"
  set_charts_data -name property_type_data -data $name -value {}
  set value3 [get_charts_data -name property_type_data -data $name]
  echo "Reset: $name $value3"
  if {$value1 != $value3} { echo "$name : RESET MISMATCH" }
  echo "---"
}

foreach name $names {
  set value [get_charts_data -name property_type_data -data $name]
  echo "Init: $name $value"

  if       {$name == "CQChartsAlpha"} {
    test_prop_data $name 0.5
  } elseif {$name == "CQChartsAngle"} {
    test_prop_data $name 45
  } elseif {$name == "CQChartsArea"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsArrowData"} {
    test_prop_data $name "line_width=1"
  } elseif {$name == "CQChartsAxisSide"} {
    test_prop_data $name "TOP_RIGHT"
  } elseif {$name == "CQChartsAxisTickLabelPlacement"} {
    test_prop_data $name "BETWEEN"
  } elseif {$name == "CQChartsAxisValueType"} {
    test_prop_data $name "DATE"
  } elseif {$name == "CQChartsBoxData"} {
    test_prop_data $name "visible=1"
  } elseif {$name == "CQChartsColor"} {
    test_prop_data $name red
  } elseif {$name == "CQChartsColorMap"} {
    test_prop_data $name {{0.0 red} {1.0 blue}}
  } elseif {$name == "CQChartsColorStops"} {
    test_prop_data $name {{10 0.0} {20 0.8} {20 0.8} {100 1.0}}
  } elseif {$name == "CQChartsColumn"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsColumnNum"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsColumns"} {
    test_prop_data $name {1 2}
  } elseif {$name == "CQChartsConnectionList"} {
    test_prop_data $name "{ { 0 1 } {1 2} }"
  } elseif {$name == "CQChartsFillData"} {
    test_prop_data $name "filled=1"
  } elseif {$name == "CQChartsFillPattern"} {
    test_prop_data $name VERT
  } elseif {$name == "CQChartsFillUnderPos"} {
    test_prop_data $name MAX
  } elseif {$name == "CQChartsFillUnderSide"} {
    test_prop_data $name BELOW
  } elseif {$name == "CQChartsFont"} {
    test_prop_data $name fixed
  } elseif {$name == "CQChartsGeom::BBox"} {
    test_prop_data $name {0 0 1 1}
  } elseif {$name == "CQChartsGeom::Point"} {
    test_prop_data $name {1 1}
  } elseif {$name == "CQChartsImage"} {
    test_prop_data $name test.png
  } elseif {$name == "CQChartsKeyLocation"} {
    test_prop_data $name BOTTOM_LEFT
  } elseif {$name == "CQChartsKeyPressBehavior"} {
    test_prop_data $name SELECT
  } elseif {$name == "CQChartsLength"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsLineCap"} {
    test_prop_data $name flat
  } elseif {$name == "CQChartsLineDash"} {
    test_prop_data $name "4 4"
  } elseif {$name == "CQChartsLineData"} {
    test_prop_data $name "visible=1"
  } elseif {$name == "CQChartsLineJoin"} {
    test_prop_data $name miter
  } elseif {$name == "CQChartsMargin"} {
    test_prop_data $name {1 1 1 1}
  } elseif {$name == "CQChartsModelColumn"} {
    test_prop_data $name 1:1
  } elseif {$name == "CQChartsModelInd"} {
    test_prop_data $name "1"
  } elseif {$name == "CQChartsModelIndex"} {
    test_prop_data $name "{} 0 1"
  } elseif {$name == "CQChartsNamePair"} {
    test_prop_data $name "a/b"
  } elseif {$name == "CQChartsObjRef"} {
    test_prop_data $name a
  } elseif {$name == "CQChartsObjRefPos"} {
    test_prop_data $name "0 0"
  } elseif {$name == "CQChartsOptBool"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsOptInt"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsOptLength"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsOptPosition"} {
    test_prop_data $name {1 1}
  } elseif {$name == "CQChartsOptReal"} {
    test_prop_data $name 1
  } elseif {$name == "CQChartsOptRect"} {
    test_prop_data $name {0 0 1 1}
  } elseif {$name == "CQChartsOptString"} {
    test_prop_data $name hello
  } elseif {$name == "CQChartsPaletteName"} {
    test_prop_data $name red_green
  } elseif {$name == "CQChartsPath"} {
    test_prop_data $name "M 0 0 L 10 10"
  } elseif {$name == "CQChartsPoints"} {
    test_prop_data $name "{0 0} {1 1}"
  } elseif {$name == "CQChartsPolygon"} {
    test_prop_data $name {{0 0} {1 0} {2 1}}
  } elseif {$name == "CQChartsPolygonList"} {
    test_prop_data $name {{{0 0} {1 0} {2 1}} {{1 1} {2 1} {3 2}}}
  } elseif {$name == "CQChartsPosition"} {
    test_prop_data $name {1 1}
  } elseif {$name == "CQChartsReals"} {
    test_prop_data $name "0 1"
  } elseif {$name == "CQChartsRect"} {
    test_prop_data $name {0 0 1 1}
  } elseif {$name == "CQChartsShapeData"} {
    test_prop_data $name "visible=1"
  } elseif {$name == "CQChartsShapeType"} {
    test_prop_data $name DIAMOND
  } elseif {$name == "CQChartsSides"} {
    test_prop_data $name "tl"
  } elseif {$name == "CQChartsStrokeData"} {
    test_prop_data $name "stroked=1"
  } elseif {$name == "CQChartsStyle"} {
    test_prop_data $name "fill:red; stroke:blue"
  } elseif {$name == "CQChartsSymbol"} {
    test_prop_data $name circle
  } elseif {$name == "CQChartsSymbolData"} {
    test_prop_data $name "visible=1"
  } elseif {$name == "CQChartsSymbolSizeMap"} {
    test_prop_data $name "{0 10} {1 12}"
  } elseif {$name == "CQChartsSymbolType"} {
    test_prop_data $name "hexagon"
  } elseif {$name == "CQChartsSymbolTypeMap"} {
    test_prop_data $name "{0 hexagon} {2 circle}"
  } elseif {$name == "CQChartsTextBoxData"} {
    test_prop_data $name "visible=1"
  } elseif {$name == "CQChartsTextData"} {
    test_prop_data $name "visible=1"
  } elseif {$name == "CQChartsThemeName"} {
    test_prop_data $name theme1
  } elseif {$name == "CQChartsTitleLocation"} {
    test_prop_data $name BOTTOM
  } elseif {$name == "CQChartsUnits"} {
    test_prop_data $name px
  } elseif {$name == "CQChartsValueList"} {
    test_prop_data $name {1 2 3}
  } elseif {$name == "CQChartsWidget"} {
    test_prop_data $name DESKTOP
  } else {
    echo "Invalid name : $name"
  }
}

exit
