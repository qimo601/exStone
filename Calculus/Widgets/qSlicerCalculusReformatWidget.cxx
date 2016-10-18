/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Michael Jeulin-Lagarrigue, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// QT includes
#include <QMenu>
#include <QString>
#include <QDebug>
#include <QTime>
#include <QTimer>
// SlicerQt includes
#include "vtkMRMLSliceNode.h"

#include "qSlicerCalculusReformatWidget.h"
#include "ui_qSlicerCalculusReformatWidget.h"

// MRML includes
#include "vtkMRMLApplicationLogic.h"
#include "vtkMRMLCameraNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSliceCompositeNode.h"
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLSliceLayerLogic.h"


// VTK includes
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkTransform.h>
#include <vtkImageReslice.h>
#include <vtkMRMLTransformNode.h>
#include <vtkGeneralTransform.h>
//***
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
//------------------------------------------------------------------------------
class qSlicerCalculusReformatWidgetPrivate :
public Ui_qSlicerCalculusReformatWidget
{
  Q_DECLARE_PUBLIC(qSlicerCalculusReformatWidget);
protected:
  qSlicerCalculusReformatWidget* const q_ptr;

public:
  qSlicerCalculusReformatWidgetPrivate(
    qSlicerCalculusReformatWidget& object);
  virtual void setupUi(qSlicerCalculusReformatWidget*);
  /// Update the widget interface
  void updateUi();

  /// Update the visibility controllers
  void updateVisibilityControllers();

  /// Update slice offset range and resolution (increment)
  void updateOffsetSlidersGroupBox();

  /// Update the origin position
  void updateOriginCoordinates();

  /// Update orientation selector state
  void updateOrientationGroupBox();

  /// Reset the slider
  void resetSlider(qMRMLLinearTransformSlider*);

  /// Setup the reformate option menu associated to the button
  void setupReformatOptionsMenu();

  QButtonGroup* OriginCoordinateReferenceButtonGroup;
  vtkMRMLSliceNode* MRMLSliceNode;
  vtkMRMLSliceLogic* MRMLSliceLogic;
  double LastRotationValues[3]; // LR, PA, IS
};

//------------------------------------------------------------------------------
// qSlicerCalculusReformatWidgetPrivate methods

//------------------------------------------------------------------------------
qSlicerCalculusReformatWidgetPrivate::
qSlicerCalculusReformatWidgetPrivate(
  qSlicerCalculusReformatWidget& object)
  : q_ptr(&object)
{
  this->OriginCoordinateReferenceButtonGroup = 0;
  this->MRMLSliceNode = 0;
  this->MRMLSliceLogic = 0;
  this->LastRotationValues[qSlicerCalculusReformatWidget::axisX] = 0;
  this->LastRotationValues[qSlicerCalculusReformatWidget::axisY] = 0;
  this->LastRotationValues[qSlicerCalculusReformatWidget::axisZ] = 0;
}
//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate::setupReformatOptionsMenu()
{
  Q_Q(qSlicerCalculusReformatWidget);

  QMenu* reformatMenu =
    new QMenu(q->tr("Reformat"),this->ShowReformatWidgetToolButton);

  reformatMenu->addAction(this->actionLockNormalToCamera);

  QObject::connect(this->actionLockNormalToCamera, SIGNAL(triggered(bool)),
                   q, SLOT(onLockReformatWidgetToCamera(bool)));

  this->ShowReformatWidgetToolButton->setMenu(reformatMenu);
}
// --------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate
::setupUi(qSlicerCalculusReformatWidget* widget)
{
	this->Ui_qSlicerCalculusReformatWidget::setupUi(widget);
}
//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate::updateUi()
{
  this->updateVisibilityControllers();
  this->updateOffsetSlidersGroupBox();
  this->updateOriginCoordinates();
  this->updateOrientationGroupBox();
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate::updateVisibilityControllers()
{
  // Check slice visibility
  bool wasVisibilityCheckBoxBlocking =
    this->VisibilityCheckBox->blockSignals(true);

  this->VisibilityCheckBox->setEnabled(this->MRMLSliceNode != 0);

  int visibility =
    (this->MRMLSliceNode) ? this->MRMLSliceNode->GetSliceVisible() : 0;
  this->VisibilityCheckBox->setChecked(visibility);

  this->VisibilityCheckBox->blockSignals(wasVisibilityCheckBoxBlocking);

  // Check reformat widget visibility
  bool wasVisibilityReformatWidgetCheckBoxBlocking =
    this->ShowReformatWidgetToolButton->blockSignals(true);
  bool wasLockReformatWidgetCheckBoxBlocking =
    this->actionLockNormalToCamera->blockSignals(true);
  bool wasLockReformatWidgetCheckBoxButtonBlocking =
    this->NormalToCameraCheckablePushButton->blockSignals(true);

  this->ShowReformatWidgetToolButton->setEnabled(this->MRMLSliceNode != 0);

  int widgetVisibility =
    (this->MRMLSliceNode) ? this->MRMLSliceNode->GetWidgetVisible() : 0;
  int lockWidgetNormal = (this->MRMLSliceNode) ?
    this->MRMLSliceNode->GetWidgetNormalLockedToCamera() : 0;

  this->ShowReformatWidgetToolButton->setChecked(widgetVisibility);
  this->actionLockNormalToCamera->setChecked(lockWidgetNormal);
  this->NormalToCameraCheckablePushButton->setChecked(lockWidgetNormal);
  this->NormalToCameraCheckablePushButton->setCheckState(
    (lockWidgetNormal) ? Qt::Checked : Qt::Unchecked);

  this->ShowReformatWidgetToolButton->blockSignals(
    wasVisibilityReformatWidgetCheckBoxBlocking);
  this->actionLockNormalToCamera->blockSignals(
    wasLockReformatWidgetCheckBoxBlocking);
  this->NormalToCameraCheckablePushButton->blockSignals(
    wasLockReformatWidgetCheckBoxButtonBlocking);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate::updateOffsetSlidersGroupBox()
{
  if (!this->MRMLSliceNode || !this->MRMLSliceLogic)
    {
    return;
    }

  bool wasBlocking = this->OffsetSlider->blockSignals(true);

  // Set the scale increments to match the z spacing (rotated into slice space)
  const double * sliceSpacing =
    this->MRMLSliceLogic->GetLowestVolumeSliceSpacing();
  Q_ASSERT(sliceSpacing);
  double offsetResolution = sliceSpacing ? sliceSpacing[2] : 0;
  this->OffsetSlider->setSingleStep(offsetResolution);
  this->OffsetSlider->setPageStep(offsetResolution);

  // Set slice offset range to match the field of view
  // Calculate the number of slices in the current range
  double sliceBounds[6] = {0, -1, 0, -1, 0, -1};
  this->MRMLSliceLogic->GetLowestVolumeSliceBounds(sliceBounds);
  Q_ASSERT(sliceBounds[4] <= sliceBounds[5]);
  this->OffsetSlider->setRange(sliceBounds[4], sliceBounds[5]);

  // Update slider position
  this->OffsetSlider->setValue(this->MRMLSliceLogic->GetSliceOffset());
  this->OffsetSlider->blockSignals(wasBlocking);
}

void qSlicerCalculusReformatWidgetPrivate::updateOriginCoordinates()
{
  Q_Q(qSlicerCalculusReformatWidget);

  vtkSlicerReformatLogic* reformatLogic =
    vtkSlicerReformatLogic::SafeDownCast(q->logic());

  if (!this->MRMLSliceNode || !reformatLogic)
    {
    return;
    }

  // Block signals
  //bool wasOnPlaneXBlocking = this->OnPlaneXdoubleSpinBox->blockSignals(true);
  //bool wasOnPlaneYBlocking = this->OnPlaneYdoubleSpinBox->blockSignals(true);
  bool wasInVolumeBlocking = this->InVolumeCoordinatesWidget->blockSignals(true);

  // Update volumes extremums
  double volumeBounds[6] = {0, 0, 0, 0, 0, 0};
  reformatLogic->GetVolumeBounds(this->MRMLSliceNode, volumeBounds);

  /// TODO: set min/max per element
  double minimum = qMin(volumeBounds[0], qMin(volumeBounds[2], volumeBounds[4]));
  double maximum = qMax(volumeBounds[1], qMax(volumeBounds[3], volumeBounds[5]));
  this->InVolumeCoordinatesWidget->setMinimum(minimum);
  this->InVolumeCoordinatesWidget->setMaximum(maximum);

  // TODO : Update plane extremums
  /*
  double sliceBounds[6] = {0, 0, 0, 0, 0, 0};
  this->MRMLSliceLogic->GetLowestVolumeSliceBounds(sliceBounds);

  this->OnPlaneXdoubleSpinBox->setMinimum(sliceBounds[0]);
  this->OnPlaneXdoubleSpinBox->setMaximum(sliceBounds[1]);
  this->OnPlaneYdoubleSpinBox->setMinimum(sliceBounds[2]);
  this->OnPlaneYdoubleSpinBox->setMaximum(sliceBounds[3]);
  */

  // Update volumes origin coordinates
  vtkMatrix4x4* sliceToRAS = this->MRMLSliceNode->GetSliceToRAS();
  this->InVolumeCoordinatesWidget->setCoordinates(sliceToRAS->GetElement(0,3),
                                                  sliceToRAS->GetElement(1,3),
                                                  sliceToRAS->GetElement(2,3));

  // TODO : Update plane origin coordinates

  // Reset signals blocking
  //this->OnPlaneXdoubleSpinBox->blockSignals(wasOnPlaneXBlocking);
  //this->OnPlaneYdoubleSpinBox->blockSignals(wasOnPlaneYBlocking);
  this->InVolumeCoordinatesWidget->blockSignals(wasInVolumeBlocking);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate::updateOrientationGroupBox()
{
  if (!this->MRMLSliceNode)
    {
    this->SliceOrientationSelector->setCurrentIndex(-1);
    return;
    }

  // Update the selector
  int index = this->SliceOrientationSelector->findText(
      QString::fromStdString(this->MRMLSliceNode->GetOrientationString()));
  Q_ASSERT(index>=0 && index <=4);
  this->SliceOrientationSelector->setCurrentIndex(index);

  // Update the normal spinboxes
  bool wasNormalBlocking = this->NormalCoordinatesWidget->blockSignals(true);

  double normal[3];
  vtkMatrix4x4* sliceToRAS = this->MRMLSliceNode->GetSliceToRAS();

  normal[0] = sliceToRAS->GetElement(0,2);
  normal[1] = sliceToRAS->GetElement(1,2);
  normal[2] = sliceToRAS->GetElement(2,2);

  this->NormalCoordinatesWidget->setCoordinates(normal);
  this->NormalCoordinatesWidget->blockSignals(wasNormalBlocking);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidgetPrivate::
resetSlider(qMRMLLinearTransformSlider* slider)
{
  bool wasSliderBlocking = slider->blockSignals(true);
  slider->reset();

  if (slider == this->LRSlider)
    {
    this->LastRotationValues[qSlicerCalculusReformatWidget::axisX] = slider->value();
    }
  else if (slider == this->PASlider)
    {
    this->LastRotationValues[qSlicerCalculusReformatWidget::axisY] = slider->value();
    }
  else if (slider == this->ISSlider)
    {
    this->LastRotationValues[qSlicerCalculusReformatWidget::axisZ] = slider->value();
    }

  slider->blockSignals(wasSliderBlocking);
}

//------------------------------------------------------------------------------
// qSlicerCalculusReformatWidget methods

//------------------------------------------------------------------------------
qSlicerCalculusReformatWidget::qSlicerCalculusReformatWidget(
  QWidget* _parent) : Superclass( _parent ),
  d_ptr( new qSlicerCalculusReformatWidgetPrivate(*this) )
{
	setup();
	m_lrTimerId =0;//lr方向旋转，计时器
	m_lrTimerCount=0;//lr执行次数
	m_paTimerId=0;//pa方向旋转，计时器
	m_paTimerCount=0;//pa执行次数
}

//------------------------------------------------------------------------------
qSlicerCalculusReformatWidget::~qSlicerCalculusReformatWidget()
{
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setup()
{
  Q_D(qSlicerCalculusReformatWidget);
  d->setupUi(this);
  this->Superclass::setup();
  // Populate the Linked menu
  d->setupReformatOptionsMenu();

  // Connect node selector with  itself
  this->connect(d->VisibilityCheckBox,
                   SIGNAL(toggled(bool)),
                   this, SLOT(onSliceVisibilityChanged(bool)));
  this->connect(d->ShowReformatWidgetToolButton,
                   SIGNAL(toggled(bool)),
                   this, SLOT(onReformatWidgetVisibilityChanged(bool)));

  this->connect(d->SliceNodeSelector,
                SIGNAL(currentNodeChanged(vtkMRMLNode*)),
                SLOT(onNodeSelected(vtkMRMLNode*)));

  // Connect Slice offset slider
  this->connect(d->OffsetSlider, SIGNAL(valueChanged(double)),
                this, SLOT(setSliceOffsetValue(double)), Qt::QueuedConnection);
  this->connect(d->OffsetSlider, SIGNAL(valueIsChanging(double)),
                this, SLOT(onTrackSliceOffsetValueChanged(double)),
                Qt::QueuedConnection);

  // Add origin coordinate reference button to a button group
  d->OriginCoordinateReferenceButtonGroup =
    new QButtonGroup(d->OriginCoordinateReferenceButtonGroup);
  d->OriginCoordinateReferenceButtonGroup->addButton(d->OnPlaneRadioButton,
    qSlicerCalculusReformatWidget::ONPLANE);
  d->OriginCoordinateReferenceButtonGroup->addButton(d->InVolumeRadioButton,
    qSlicerCalculusReformatWidget::INVOLUME);

  // Plane coordinate system is not supported for now
  d->CoordinateReferenceGroupBox->setHidden(true);
  d->InVolumeRadioButton->setChecked(true);
  d->OnPlaneGroupBox->setHidden(true);

  // Connect button group
  this->connect(d->OriginCoordinateReferenceButtonGroup,
                SIGNAL(buttonPressed(int)),
                SLOT(onOriginCoordinateReferenceButtonPressed(int)));

  // Connect World Coordinates of origin spinBoxes
  this->connect(d->InVolumeCoordinatesWidget, SIGNAL(coordinatesChanged(double*)),
                this, SLOT(setWorldPosition(double*)));

  // Connect Orientation selector
  this->connect(d->SliceOrientationSelector, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(onSliceOrientationChanged(QString)));

  // Connect the recenter
  this->connect(d->CenterPushButton, SIGNAL(pressed()),
                this, SLOT(centerSliceNode()));

  // Connect slice normal spinBoxes
  this->connect(d->NormalCoordinatesWidget, SIGNAL(coordinatesChanged(double*)),
                this, SLOT(setSliceNormal(double*)));

  // Connect slice normal pushButtons
  this->connect(d->NormalXPushButton, SIGNAL(pressed()),
                this, SLOT(setNormalToAxisX()));
  this->connect(d->NormalYPushButton, SIGNAL(pressed()),
                this, SLOT(setNormalToAxisY()));
  this->connect(d->NormalZPushButton, SIGNAL(pressed()),
                this, SLOT(setNormalToAxisZ()));

  QObject::connect(d->NormalToCameraCheckablePushButton, SIGNAL(clicked()),
                   this, SLOT(setNormalToCamera()));
  QObject::connect(d->NormalToCameraCheckablePushButton,
                   SIGNAL(checkBoxToggled(bool)),
                   this, SLOT(onLockReformatWidgetToCamera(bool)));

  // Connect Slice rotation sliders
  this->connect(d->LRSlider, SIGNAL(valueChanged(double)),
                this, SLOT(onSliderRotationChanged(double)));
  this->connect(d->PASlider, SIGNAL(valueChanged(double)),
                this, SLOT(onSliderRotationChanged(double)));
  this->connect(d->ISSlider, SIGNAL(valueChanged(double)),
                this, SLOT(onSliderRotationChanged(double)));
  qDebug() << "void qSlicerCalculusReformatWidget::setup()";
}
//------------------------------------------------------------------------------
//自定义 设置ReformatLogic
void qSlicerCalculusReformatWidget::setReformatLogic(vtkSlicerReformatLogic* logic)
{
	m_reformatLogic = logic;
}
//------------------------------------------------------------------------------
//自定义 返回当前的logic，覆盖父类qSlicerAbstractModuleRepresentation 的logic
vtkMRMLAbstractLogic* qSlicerCalculusReformatWidget::logic()
{
	return vtkMRMLAbstractLogic::SafeDownCast(m_reformatLogic);
}
//------------------------------------------------------------------------------
//自定义 设置本module的Logic
void qSlicerCalculusReformatWidget::setCalculusLogic(vtkSlicerCalculusLogic* logic)
{
	m_calculusLogic = logic;
}
//自定义 获取本module的Logic
vtkSlicerCalculusLogic* qSlicerCalculusReformatWidget::getCalculusLogic()
{
	return m_calculusLogic;
}
//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setVtkMRMLVolumeNode(vtkMRMLVolumeNode* node)
{
	m_vtkMRMLVolumeNode = node;
}
vtkMRMLVolumeNode* qSlicerCalculusReformatWidget::getVtkMRMLVolumeNode()
{
	return m_vtkMRMLVolumeNode;
}
//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setVtkMRMLSliceNodeRed(vtkMRMLSliceNode* node)
{
	m_vtkMRMLSliceNodeRed = node;
}
vtkMRMLSliceNode* qSlicerCalculusReformatWidget::getVtkMRMLSliceNodeRed()
{
	return m_vtkMRMLSliceNodeRed;
}
//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
onMRMLSliceNodeModified(vtkObject* caller)
{
  Q_D(qSlicerCalculusReformatWidget);

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(caller);
  if (!sliceNode)
    {
    return;
    }

  d->updateUi();
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::onNodeSelected(vtkMRMLNode* node)
{
  Q_D(qSlicerCalculusReformatWidget);

  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);

  // Listen for SliceNode changes
  this->qvtkReconnect(d->MRMLSliceNode, sliceNode,
    vtkCommand::ModifiedEvent,
    this, SLOT(onMRMLSliceNodeModified(vtkObject*)));

  d->MRMLSliceNode = sliceNode;
  d->MRMLSliceLogic =
    this->logic()->GetMRMLApplicationLogic()->GetSliceLogic(d->MRMLSliceNode);

  d->updateUi();
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::onSliceVisibilityChanged(bool visible)
{
  std::cout << "onSliceVisibilityChanged" << std::endl;
  Q_D(qSlicerCalculusReformatWidget);
  if (!d->MRMLSliceNode)
    {
    return;
    }

  d->MRMLSliceNode->SetSliceVisible(visible);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
onReformatWidgetVisibilityChanged(bool visible)
{
  Q_D(qSlicerCalculusReformatWidget);
  if (!d->MRMLSliceNode)
    {
    return;
    }

  if (visible)
    {
    d->MRMLSliceNode->SetSliceVisible(visible);
    }

  d->MRMLSliceNode->SetWidgetVisible(visible);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::onLockReformatWidgetToCamera(bool lock)
{
  Q_D(qSlicerCalculusReformatWidget);
  if (!d->MRMLSliceNode)
    {
    return;
    }

  d->MRMLSliceNode->SetWidgetNormalLockedToCamera(lock);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
onOriginCoordinateReferenceButtonPressed(int ref)
{
  Q_D(qSlicerCalculusReformatWidget);

  d->OnPlaneGroupBox->setHidden(ref == qSlicerCalculusReformatWidget::INVOLUME);
  d->InVolumeCoordinatesWidget->setHidden(ref != qSlicerCalculusReformatWidget::INVOLUME);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
setSliceOffsetValue(double offset)
{
  Q_D(qSlicerCalculusReformatWidget);
  if (!d->MRMLSliceLogic)
    {
    return;
    }

  d->MRMLSliceLogic->StartSliceOffsetInteraction();
  d->MRMLSliceLogic->SetSliceOffset(offset);
  d->MRMLSliceLogic->EndSliceOffsetInteraction();
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
onTrackSliceOffsetValueChanged(double offset)
{
  Q_D(qSlicerCalculusReformatWidget);
  if (!d->MRMLSliceLogic)
    {
    return;
    }

  d->MRMLSliceLogic->StartSliceOffsetInteraction();
  d->MRMLSliceLogic->SetSliceOffset(offset);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setWorldPosition(double* worldCoordinates)
{
  Q_D(qSlicerCalculusReformatWidget);

  vtkSlicerReformatLogic* reformatLogic =
    vtkSlicerReformatLogic::SafeDownCast(this->logic());

  if (!d->MRMLSliceNode || !reformatLogic)
    {
    return;
    }

  // Insert the widget translation
  reformatLogic->SetSliceOrigin(d->MRMLSliceNode, worldCoordinates);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setSliceNormal(double x, double y, double z)
{
  double sliceNormal[3] = {x,y,z};
  this->setSliceNormal(sliceNormal);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setNormalToCamera()
{
  vtkSlicerReformatLogic* reformatLogic =
    vtkSlicerReformatLogic::SafeDownCast(this->logic());

  if (!reformatLogic)
    {
    return;
    }

  // NOTE: We use the first Camera because there is no notion of active scene
  // Code to be changed when methods avaible.
  vtkMRMLCameraNode* cameraNode = vtkMRMLCameraNode::SafeDownCast(
    reformatLogic->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLCameraNode"));

  if (!cameraNode)
    {
    return;
    }

  double camNormal[3];
  cameraNode->GetCamera()->GetViewPlaneNormal(camNormal);
  this->setSliceNormal(camNormal);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setNormalToAxisX()
{
  this->onSliceNormalToAxisChanged(axisX);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setNormalToAxisY()
{
  this->onSliceNormalToAxisChanged(axisY);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setNormalToAxisZ()
{
  this->onSliceNormalToAxisChanged(axisZ);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::onSliceNormalToAxisChanged(AxesReferenceType
                                                             axis)
{
  double sliceNormal[3];
  sliceNormal[0] = (axis == axisX) ? 1. : 0.;
  sliceNormal[1] = (axis == axisY) ? 1. : 0.;
  sliceNormal[2] = (axis == axisZ) ? 1. : 0.;

  // Insert the widget rotation
  this->setSliceNormal(sliceNormal);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setSliceNormal(double* sliceNormal)
{
  Q_D(qSlicerCalculusReformatWidget);

  vtkSlicerReformatLogic* reformatLogic =
    vtkSlicerReformatLogic::SafeDownCast(this->logic());

  if (!d->MRMLSliceNode || !reformatLogic)
    {
    return;
    }

  // Reset rotation sliders
  d->resetSlider(d->LRSlider);
  d->resetSlider(d->PASlider);
  d->resetSlider(d->ISSlider);

  double normalizedSliceNormal[3] = {sliceNormal[0], sliceNormal[1], sliceNormal[2]};
  vtkMath::Normalize(normalizedSliceNormal);

  // Insert the widget rotation
  reformatLogic->SetSliceNormal(d->MRMLSliceNode, normalizedSliceNormal);
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
onSliceOrientationChanged(const QString& orientation)
{
  Q_D(qSlicerCalculusReformatWidget);

  if (!d->MRMLSliceNode)
    {
    return;
    }

  // Reset the Rotation Sliders
  d->resetSlider(d->LRSlider);
  d->resetSlider(d->PASlider);
  d->resetSlider(d->ISSlider);

#ifndef QT_NO_DEBUG
  QStringList expectedOrientation;
  expectedOrientation << tr("Axial") << tr("Sagittal")
                      << tr("Coronal") << tr("Reformat");
  Q_ASSERT(expectedOrientation.contains(orientation));
#endif

  d->MRMLSliceNode->SetOrientation(orientation.toLatin1());
  d->MRMLSliceNode->SetOrientationString(orientation.toLatin1());
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::
onSliderRotationChanged(double rotation)
{
  Q_D(qSlicerCalculusReformatWidget);

  vtkNew<vtkTransform> transform;
  transform->SetMatrix(d->MRMLSliceNode->GetSliceToRAS());

  if (this->sender() == d->LRSlider)
    {
    // Reset PA & IS sliders
    d->resetSlider(d->PASlider);
    d->resetSlider(d->ISSlider);

    // Rotate on LR given the angle with the last value reccorded
    transform->RotateX(rotation-d->LastRotationValues[axisX]);

    // Update last value and apply the transform
    d->LastRotationValues[axisX] = rotation;
    }
  else if (this->sender() == d->PASlider)
    {
    // Reset LR & IS sliders
    d->resetSlider(d->LRSlider);
    d->resetSlider(d->ISSlider);

    // Rotate on PA given the angle with the last value reccorded
    transform->RotateY(rotation-d->LastRotationValues[axisY]);

    // Update last value and apply the transform
    d->LastRotationValues[axisY] = rotation;
    }
  else if (this->sender() == d->ISSlider)
    {
      // Reset LR & PA sliders
      d->resetSlider(d->LRSlider);
      d->resetSlider(d->PASlider);

      // Rotate on PA given the angle with the last value reccorded
      transform->RotateZ(rotation-d->LastRotationValues[axisZ]);

      // Update last value and apply the transform
      d->LastRotationValues[axisZ] = rotation;
    }

  // Apply the transform
  d->MRMLSliceNode->GetSliceToRAS()->DeepCopy(transform->GetMatrix());
  d->MRMLSliceNode->UpdateMatrices();
}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::centerSliceNode()
{
  Q_D(qSlicerCalculusReformatWidget);

  vtkSlicerReformatLogic* reformatLogic =
    vtkSlicerReformatLogic::SafeDownCast(this->logic());

  if (!d->MRMLSliceNode || !d->MRMLSliceLogic || !reformatLogic)
    {
    return;
    }

  // TODO add the recenter given the Plane Referentiel

  // Retrieve the center given the volume bounds
  double bounds[6], center[3];
  reformatLogic->GetVolumeBounds(d->MRMLSliceNode, bounds);
  vtkSlicerReformatLogic::GetCenterFromBounds(bounds, center);

  // Apply the center
  reformatLogic->SetSliceOrigin(d->MRMLSliceNode, center);
}

//-----------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setMRMLScene(vtkMRMLScene* scene)
{
	this->Superclass::setMRMLScene(scene);
	if (scene == NULL)
	{
		return;
	}

	// observe close event so can re-add a parameters node if necessary
	qvtkReconnect(this->mrmlScene(), vtkMRMLScene::EndCloseEvent,
		this, SLOT(onEndCloseEvent()));

}

//-----------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::enter()
{
	// if there are already some
	// volumes or ROIs in the scene, they can be set up for use

	//this->onInputVolumeMRMLNodeChanged();
	//Q_D(qSlicerCalculusModuleWidget);
	//d->outputVolumeMRMLNodeComboBox->setEnabled(false);

	this->Superclass::enter();
}
//
void qSlicerCalculusReformatWidget::onEndCloseEvent()
{
	//Q_D(qSlicerCalculusReformatWidget);
	//vtkSmartPointer<vtkSlicerReformatLogic> logic = d->logic();
	//logic->reset(vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()), 0);
	//d->acqStoneBtn->setEnabled(true);
	cout << "close scene!" << endl;

}
//传递qSlicerCalculusModuleWidget的mrmlSceneChanged(vtkMRMLScene*)信号
void qSlicerCalculusReformatWidget::onMRMLSceneChanged(vtkMRMLScene* scene)
{
	//Q_D(qSlicerCalculusReformatWidget);
	//vtkSmartPointer<vtkSlicerReformatLogic> logic = d->logic();
	//logic->reset(vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()), 0);
	//d->acqStoneBtn->setEnabled(true);
	emit mrmlSceneChanged(scene);

}

//------------------------------------------------------------------------------
/**
* @brief 允许当前的窗口变形
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::enableReformat(bool enable)
{
	Q_D(qSlicerCalculusReformatWidget);
	//选择R窗口
	d->SliceNodeSelector->setCurrentNodeIndex(0);
	
	if (enable)
	{
		if (!d->ShowReformatWidgetToolButton->isChecked())//允许变形
			d->ShowReformatWidgetToolButton->toggle();
		if (!d->VisibilityCheckBox->isChecked())//允许可见
			d->VisibilityCheckBox->toggle();
		
	}
	else
	{
		if (d->ShowReformatWidgetToolButton->isChecked())//关闭变形
			d->ShowReformatWidgetToolButton->toggle();
		if (d->VisibilityCheckBox->isChecked())//关闭可见
			d->VisibilityCheckBox->toggle();
	}


	randRotate();
	
}

//------------------------------------------------------------------------------
/**
* @brief 旋转
* @param QString direction,"LR" "PA" "IS"
* @param double value: 旋转角度
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::rotate(QString direction, double value)
{
	Q_D(qSlicerCalculusReformatWidget);
	if (direction == "LR")
		d->LRSlider->setValue(value);
	else if (direction == "PA")
		d->PASlider->setValue(value);
	else if (direction == "IS")
		d->ISSlider->setValue(value);

	getSliceRawData();

}
//------------------------------------------------------------------------------
/**
* @brief 随机函数
* @param int min, int max
* @author liuzhaobang
* @date 2016-10-14
*/
int qSlicerCalculusReformatWidget::getRand(int min, int max)
{
	return min + qrand() % (max - min);
}
/**
* @brief 随机旋转
* @param int min, int max
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::randRotate()
{
	Q_D(qSlicerCalculusReformatWidget);
	//随机函数种子初始化，运行一次软件只初始化这一次，保证随机性不重复。
	QTime t;
	t = QTime::currentTime();
	qsrand(t.msec() + t.second() * 1000);
	int value = 0;
	m_lrValueList.clear();
	m_paValueList.clear();
	//任意旋转
	for (int i = 0; i < 16; i++)
	{
		value = getRand(-200, 200);
		m_lrValueList.append(value);


		value = getRand(-200, 200);
		m_paValueList.append(value);
		//d->PASlider->setValue(0);
		//d->LRSlider->setValue(0);

		/*value = getRand(-200, 200);
		rotate("PA", value);
		d->PASlider->setValue(0);
		d->LRSlider->setValue(0);*/
		
	}

	m_lrTimerId = startTimer(1000);

	//m_paTimerId = startTimer(1000);
}
/**
* @brief 定时器事件
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_lrTimerId)
	{

		//if (m_lrTimerCount < m_lrValueList.count())
		if (m_lrTimerCount < 1)//临时一次
		{
			rotate("LR", m_lrValueList[m_lrTimerCount]);
			m_lrTimerCount++;
			qDebug() << "qSlicerCalculusReformatWidget::timerEvent,m_lrTimerCount:" << m_lrTimerCount;
		}

		else
		{
			m_lrTimerCount = 0;
			killTimer(m_lrTimerId);
			m_lrValueList.clear();

		}
	}
	else if (event->timerId() == m_paTimerId)
	{
		
		//if (m_paTimerCount < m_paValueList.count())
		if (m_paTimerCount < 1)
		{
			rotate("PA", m_paValueList[m_paTimerCount]);
			m_paTimerCount++;
			qDebug() << "qSlicerCalculusReformatWidget::timerEvent,m_paTimerCount:" << m_paTimerCount;

		}

		else
		{
			m_paTimerCount = 0;
			killTimer(m_paTimerId);
			m_paValueList.clear();
		}
	}
}
/**
* @brief 获取当切片数据
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::getSliceRawData()
{
	Q_D(qSlicerCalculusReformatWidget);

	
	d->MRMLSliceLogic =
		this->logic()->GetMRMLApplicationLogic()->GetSliceLogic(this->m_vtkMRMLSliceNodeRed);
	/*d->MRMLSliceLogic =
		this->logic()->GetMRMLApplicationLogic()->GetSliceLogic(d->MRMLSliceNode);*/
	vtkMRMLSliceLayerLogic* sliceLayerLogic = d->MRMLSliceLogic->GetBackgroundLayer();
	vtkSmartPointer<vtkImageReslice> reslice = sliceLayerLogic->GetReslice();





	//获取单帧切片数据
	if (m_calculusLogic)
		m_calculusLogic->acqSliceData(reslice.GetPointer());

////	int extent[6];
////	this->m_vtkMRMLVolumeNode->GetImageData()->GetExtent(extent);
////	vtkNew<vtkMatrix4x4> rasToIJK;
////
////	vtkNew<vtkTransform> transform;
////	//vtkTransform *transform = vtkTransform::New();
////	//transform->SetMatrix(d->MRMLSliceNode->GetSliceToRAS());
////	//reslice->SetResliceTransform(transform.GetPointer());
////
////
////
////
////	
////	reslice->GenerateStencilOutputOn();
////
////	vtkNew<vtkGeneralTransform> resampleXform;
////	resampleXform->Identity();
////	resampleXform->PostMultiply();
////
////	this->m_vtkMRMLVolumeNode->GetRASToIJKMatrix(rasToIJK.GetPointer());
////
////	vtkNew<vtkMatrix4x4> IJKToRAS;
////	IJKToRAS->DeepCopy(rasToIJK.GetPointer());
////	IJKToRAS->Invert();
////	transform->Inverse();
////
////	resampleXform->Concatenate(IJKToRAS.GetPointer());
////	resampleXform->Concatenate(transform.GetPointer());
////	resampleXform->Concatenate(rasToIJK.GetPointer());
////
////	// vtkImageReslice works faster if the input is a linear transform, so try to convert it
////	// to a linear transform
////	vtkNew<vtkTransform> linearResampleXform;
////	if (vtkMRMLTransformNode::IsGeneralTransformLinear(resampleXform.GetPointer(), linearResampleXform.GetPointer()))
////	{
////		reslice->SetResliceTransform(linearResampleXform.GetPointer());
////	}
////	else
////	{
////		reslice->SetResliceTransform(resampleXform.GetPointer());
////	}
////
////#if (VTK_MAJOR_VERSION <= 5)
////	reslice->SetInput(this->ImageData);
////#else
////	reslice->SetInputConnection(this->m_vtkMRMLVolumeNode->GetImageDataConnection());
////#endif
////	reslice->SetInterpolationModeToLinear();//线性
////	reslice->SetBackgroundColor(0, 0, 0, 0);
////	reslice->AutoCropOutputOff();//关闭
////	reslice->SetOptimization(1);
////
////	reslice->SetOutputOrigin(this->m_vtkMRMLVolumeNode->GetImageData()->GetOrigin());
////	reslice->SetOutputSpacing(this->m_vtkMRMLVolumeNode->GetImageData()->GetSpacing());
////	reslice->SetOutputDimensionality(3);
////
////
////	reslice->SetOutputExtent(extent);
////
////	reslice->Update();
////#if (VTK_MAJOR_VERSION <= 5)
////	if (reslice->GetOutput(1))
////	{
////		reslice->GetOutput(1)->SetUpdateExtentToWholeExtent();
////	}
////#endif
////
//	//vtkNew<vtkImageData> resampleImage;
//	//resampleImage->DeepCopy(reslice->GetOutput());
//
//	//this->m_vtkMRMLVolumeNode->SetAndObserveImageData(resampleImage.GetPointer());
//	
//
//	int extent[6];
//	double spacing[3];
//	double origin[3];
//	this->m_vtkMRMLVolumeNode->GetImageData()->GetExtent(extent);
//	this->m_vtkMRMLVolumeNode->GetImageData()->GetOrigin(origin);
//	this->m_vtkMRMLVolumeNode->GetImageData()->GetSpacing(spacing);
//
//	vtkNew<vtkImageReslice> reslice;
//	
//	//reslice->SetOutputOrigin(origin);
//	//reslice->SetOutputSpacing(spacing);
//	reslice->SetOutputDimensionality(2);
//	//reslice->SetOutputExtent(extent);
//
//	/*double center[3];
//	center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
//	center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
//	center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
//	double axialElements[16] = {
//		1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1 };
//	vtkSmartPointer<vtkMatrix4x4> resliceAxes =
//		vtkSmartPointer<vtkMatrix4x4>::New();
//	resliceAxes->DeepCopy(axialElements);
//
//	resliceAxes->SetElement(0, 3, center[0]);
//	resliceAxes->SetElement(1, 3, center[1]);
//	resliceAxes->SetElement(2, 3, center[2]);
//	reslice->SetResliceAxes(resliceAxes);*/
//
//	reslice->SetInputConnection(0,this->m_vtkMRMLVolumeNode->GetImageDataConnection());
//	reslice->SetInterpolationModeToLinear();//线性
//	reslice->SetBackgroundColor(0, 0, 0, 0);
//	reslice->AutoCropOutputOff();//关闭
//	reslice->SetOptimization(1);
//	
//	reslice->Update();
//
//
//
//	//获取单帧切片数据
//	if (m_calculusLogic)
//		m_calculusLogic->acqSliceData(reslice.GetPointer());
//
//
//
//	//vtkSmartPointer<vtkLookupTable> colorTable =
//	//	vtkSmartPointer<vtkLookupTable>::New();
//	//colorTable->SetRange(0, 1000);
//	//colorTable->SetValueRange(0.0, 1.0);
//	//colorTable->SetSaturationRange(0.0, 0.0);
//	//colorTable->SetRampToLinear();
//	//colorTable->Build();
//	//vtkSmartPointer<vtkImageMapToColors> colorMap =
//	//	vtkSmartPointer<vtkImageMapToColors>::New();
//	//colorMap->SetLookupTable(colorTable);
//	//colorMap->SetInputConnection(reslice->GetOutputPort());
//	//vtkSmartPointer<vtkImageActor> imgActor =
//
//	//	vtkSmartPointer<vtkImageActor>::New();
//
//	//imgActor->SetInputData(colorMap->GetOutput());
//	//vtkSmartPointer<vtkRenderer> renderer =
//	//	vtkSmartPointer<vtkRenderer>::New();
//	//renderer->AddActor(imgActor);
//	//renderer->SetBackground(.4, .5, .6);
//	//vtkSmartPointer<vtkRenderWindow> renderWindow =
//	//	vtkSmartPointer<vtkRenderWindow>::New();
//	//renderWindow->SetSize(500, 500);
//	//renderWindow->AddRenderer(renderer);
//	//vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//	//	vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	//vtkSmartPointer<vtkInteractorStyleImage> imagestyle =
//	//	vtkSmartPointer<vtkInteractorStyleImage>::New();
//	//renderWindowInteractor->SetInteractorStyle(imagestyle);
//	//renderWindowInteractor->SetRenderWindow(renderWindow);
//	//renderWindowInteractor->Initialize();
//	//renderWindowInteractor->Start();
}