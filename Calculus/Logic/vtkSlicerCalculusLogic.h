/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// .NAME vtkSlicerCalculusLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerCalculusLogic_h
#define __vtkSlicerCalculusLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerCropVolumeLogic.h"
// MRML includes
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLSliceNode.h"

// STD includes
#include <cstdlib>

#include "vtkSlicerCalculusModuleLogicExport.h"
#include "TumorSegm/AdaptiveSegment3D.h"
#include "TumorSegm/AppData.h"

// VTK includes
#include <vtkImageReslice.h>

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_CALCULUS_MODULE_LOGIC_EXPORT vtkSlicerCalculusLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerCalculusLogic *New();
  vtkTypeMacro(vtkSlicerCalculusLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  //设置当前体数据的logic
  void setVolumesLogic(vtkSlicerVolumesLogic* logic);
  vtkSlicerVolumesLogic* getVolumesLogic();

  void setCropVolumeLogic(vtkSlicerCropVolumeLogic* cropVolume);
  vtkSlicerCropVolumeLogic* getCropVolumeLogic();
  //获取当前切片的像素数据
  bool acqSliceData(vtkImageReslice* reslice);

protected:
  vtkSlicerCalculusLogic();
  virtual ~vtkSlicerCalculusLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerCalculusLogic(const vtkSlicerCalculusLogic&); // Not implemented
  void operator=(const vtkSlicerCalculusLogic&); // Not implemented

  AdaptiveSegment3D* m_seg;
  AppData m_gData;
  //体数据的logic
  vtkSlicerVolumesLogic* volumesLogic;
  //剪切后的体数据
  vtkSlicerCropVolumeLogic* cropVolumeLogic;
};

#endif


