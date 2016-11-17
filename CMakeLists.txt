cmake_minimum_required(VERSION 2.8.9)

project(exStone)
#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/exStone")
set(EXTENSION_CATEGORY "Examples")
set(EXTENSION_CONTRIBUTORS "LiuZhaoBang (Sibet)")
set(EXTENSION_DESCRIPTION "This extension is a software for the analysis of the density of urinary stones. The exStone can achieved the segmentation of urinary calculi from CT Image, and  automaticly extract the  slice's parameters  in any angle of the  urinary stones volume   as well as perpendicular to the X/Y/Z axis. The CT maximum, minimum, average, average optical density and integral density of each slice of the stone were calculated. At finally,it can support export  parameters to the Excel file.")
set(EXTENSION_ICONURL "https://www.slicer.org/w/images/e/e7/Urinary_calculi.png")
set(EXTENSION_SCREENSHOTURLS "https://www.slicer.org/w/images/6/6b/Extract_parameters_in_any_angle_.gif")

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(Calculus)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
