Range Thread: updateRange
 setModel, modelChangedSlot, setUpdatesEnabled, ..
  updateRangeAndObjs
   updateRangeAndObjsInternal
    updateRange
 updateRangeAndObjsTimerSlot
  updateRangeAndObjsInternal
   updateRange
 autoFit
  autoFitOne
   updateRangeAndObjsInternal
    updateRange
 applyDataRange
  updateRange
 initObjs
  updateRange

Objs  Thread: updateObjs
 setDataRange, setXMin, ..., updateTimerSlot, updateRangeAndObjsInternal
  updateObjs
Draw  Thread: draw

initGroupedPlotObjs
 initPlotObjs
  initObjs
   updateRange
  initObjTree

State Variables:

layersChanged_ : needs draw
