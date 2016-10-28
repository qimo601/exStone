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
#include "common.h"
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

// VTK includes
#include <vtkImageReslice.h>

//----------------朱珊珊添加----------------------
// Slicer includes
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"
#include <cstdlib>
#include "vtkSlicerCalculusModuleLogicExport.h"
//--------------与excel有关的头文件
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
#include "vtkPoints.h"
class QLabel;
class QPushButton;
#include <vtkIOStream.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
//与tableWidget有关的类
#include <QTableWidget>  
#include <QTableWidgetItem>
#include <QDialog>
#include <QAction>
#include <QApplication>
//excel有关头文件
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
using namespace std;
//----------------添加结束---------------------------------

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
	QHash<QString, double> acqSliceData(vtkImageReslice* reslice,vtkMRMLSliceNode* sliceNode, vtkMRMLVolumeNode* volumeNode);
	  //-------朱珊珊---
	  QHash<QString, double> aqcCircleData(vtkMRMLVolumeNode* input, vtkMRMLMarkupsFiducialNode* markups);
	  //获取垂直切面的slice数据
	  QHash<QString, double> acqSliceVerticalData(vtkMRMLVolumeNode* input, double offset, QString direction);
	  //-------朱珊珊--------

	  double max(double a[], int n);
	  double min(double a[], int n);
	  double aver(double a[], int n);
	  double AOD(double a[], int n, double m, double d);
	  double IOD(double a[], int n, double m, double d);

	  static double s_sliceThick;//任意角度切片厚度
	  static double s_uWater;//水的衰减系数
	  static double s_materialThick;//材料厚度
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

	//体数据的logic
	vtkSlicerVolumesLogic* volumesLogic;
	//剪切后的体数据
	vtkSlicerCropVolumeLogic* cropVolumeLogic;
};

#endif


