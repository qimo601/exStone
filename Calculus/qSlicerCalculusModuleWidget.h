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

#ifndef __qSlicerCalculusModuleWidget_h
#define __qSlicerCalculusModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerCalculusModuleExport.h"

class qSlicerCalculusModuleWidgetPrivate;
class vtkMRMLNode;

// CHEN
class vtkSlicerCalculusLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_CALCULUS_EXPORT qSlicerCalculusModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerCalculusModuleWidget(QWidget *parent=0);
  virtual ~qSlicerCalculusModuleWidget();

public slots:
//CHEN
/// Update the selection node from the combo box
void onInputVolumeMRMLNodeChanged();
void updateAcqStoneButtonState();

/// Display property button slots
///acquire the urinary calculi parameters
void onAcqStoneBtnClicked();
void on_pushButton_clicked();

/**
* @brief 场景关闭事件
* @author liuzhaobang
* @date 2016-10-14
*/
void onEndCloseEvent();
protected:
  QScopedPointer<qSlicerCalculusModuleWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);
  virtual void enter();
private:
  Q_DECLARE_PRIVATE(qSlicerCalculusModuleWidget);
  Q_DISABLE_COPY(qSlicerCalculusModuleWidget);
};

#endif
