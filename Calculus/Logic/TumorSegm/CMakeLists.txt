cmake_minimum_required(VERSION 2.8)

project(TumorSegm)
set(tumor_segm_name "${PROJECT_NAME}")

FIND_PACKAGE(VTK)
INCLUDE(${VTK_USE_FILE})
INCLUDE("${VTK_DIR}/Common/DataModel/CMakeFiles/vtkCommonDataModel.cmake")

set(tumor_segm_SRCS
  AdaptiveSegment2D.cpp
  AdaptiveSegment2D.h
  AdaptiveSegment3D.cpp
  AdaptiveSegment3D.h
  AppData.cpp
  AppData.h
  common.h
  Data2D.h
  Data2Mat.h
  Data3D.h
  Graph2D.cpp
  Graph2D.h
  Graph3D.cpp
  Graph3D.h
  GraphLinkCost.h
  HardConstraint.cpp
  HardConstraint.h
  Image2D.h
  Image3D.h
  ImageProcessing.cpp
  ImageProcessing.h
  MyBasic.h
  MyMath.cpp
  MyMath.h
  smart_assert.h
  Smoothterm.cpp
  Smoothterm.h
  StarShape.cpp
  StarShape.h
  VectOperation.h
  BK/block.h
  BK/graph.cpp
  BK/graph.h
  BK/maxflow.cpp
  )

add_library(TumorSegm STATIC ${tumor_segm_SRCS})


TARGET_LINK_LIBRARIES(vtkCommonDataModel)


