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

#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

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

#include<algorithm>
using namespace std;
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


  // Set the scale increments to match the z spacing (rotated into slice space)
  double * m_sliceSpacing;
  double m_sliceBounds[6];
  double m_offset;
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
  this->updateOriginCoordinates();//Test failed
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
  m_offset = this->MRMLSliceLogic->GetSliceOffset();
  this->OffsetSlider->setValue(m_offset);
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
	//****************Test ****************//
	//qSlicerAbstractCoreModule* reformatModule =
	//	qSlicerCoreApplication::application()->moduleManager()->module("Reformat");
	//if (reformatModule)
	//{
	//	vtkSlicerReformatLogic* reformatLogic =
	//		vtkSlicerReformatLogic::SafeDownCast(this->logic());
	//	//\BB\F1ȡReformat module\B5\C4logic
	//	this->setReformatLogic(reformatLogic);
	//}
	setup();
	//****************Test ****************//

	m_lrTimerId =0;//lr\B7\BD\CF\F2\D0\FDת\A3\AC\BC\C6ʱ\C6\F7
	m_lrTimerCount=0;//lrִ\D0д\CE\CA\FD

	m_paTimerId=0;//pa\B7\BD\CF\F2\D0\FDת\A3\AC\BC\C6ʱ\C6\F7
	m_paTimerCount=0;//paִ\D0д\CE\CA\FD

	m_verticalTimerId=0;//\B4\B9ֱ\B7\BD\CF\F2\BC\C6ʱ\C6\F7
	m_verticalTimerCount=0;//\B4\B9ֱ\B7\BD\CF\F2ִ\D0д\CE\CA\FD
	m_continueTimerId = 0;//\C1\AC\D0\F8\B2ɼ\AF\BC\C6ʱ\C6\F7
	m_continueTimerCount = 0;//\C1\AC\D0\F8\B2ɼ\AFִ\D0д\CE\CA\FD
	m_vtkMRMLScene = 0;
	m_vtkMRMLSliceNodeRed = 0;
	m_vtkMRMLVolumeNode = 0;
	qDebug() << "qSlicerCalculusReformatWidget construct class.";
	
}

//------------------------------------------------------------------------------
qSlicerCalculusReformatWidget::~qSlicerCalculusReformatWidget()
{
}

void qSlicerCalculusReformatWidget::setupSlot()
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
	this->connect(d->OffsetSlider, SIGNAL(valueChanged(double)),
		this, SLOT(getVerticalStoneSlot(double)));

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
	qDebug() << "void qSlicerCalculusReformatWidget::setupSlot()";


}

//------------------------------------------------------------------------------
void qSlicerCalculusReformatWidget::setup()
{
	//setupSlot();
	qDebug() << "void qSlicerCalculusReformatWidget::setup()";
}
//------------------------------------------------------------------------------
//\D7Զ\A8\D2\E5 \C9\E8\D6\C3ReformatLogic
void qSlicerCalculusReformatWidget::setReformatLogic(vtkSlicerReformatLogic* logic)
{
	m_reformatLogic = logic;
}
//------------------------------------------------------------------------------
//\D7Զ\A8\D2\E5 \B7\B5\BBص\B1ǰ\B5\C4logic\A3\AC\B8\B2\B8Ǹ\B8\C0\E0qSlicerAbstractModuleRepresentation \B5\C4logic
vtkMRMLAbstractLogic* qSlicerCalculusReformatWidget::logic()
{
	return vtkMRMLAbstractLogic::SafeDownCast(m_reformatLogic);
}
//------------------------------------------------------------------------------
//\D7Զ\A8\D2\E5 \C9\E8\D6ñ\BEmodule\B5\C4Logic
void qSlicerCalculusReformatWidget::setCalculusLogic(vtkSlicerCalculusLogic* logic)
{
	m_calculusLogic = logic;
}
//\D7Զ\A8\D2\E5 \BB\F1ȡ\B1\BEmodule\B5\C4Logic
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

void qSlicerCalculusReformatWidget::setVtkMRMLScene(vtkMRMLScene* scene)
{
	m_vtkMRMLScene = scene;
}
vtkMRMLScene* qSlicerCalculusReformatWidget::getVtkMRMLScene()
{
	return m_vtkMRMLScene;
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
//\B4\AB\B5\DDqSlicerCalculusModuleWidget\B5\C4mrmlSceneChanged(vtkMRMLScene*)\D0ź\C5
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
* @brief \D4\CA\D0\ED\B5\B1ǰ\B5Ĵ\B0\BFڱ\E4\D0\CE
* @param direction
*	//ѡ\D4񴰿\DA SliceNodeSelector\B5\C4 NodeIDֵ
	//"vtkMRMLSliceNodeRed"
	// "vtkMRMLSliceNodeYellow"
	//"vtkMRMLSliceNodeGreen"
	//""
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::enableReformat(bool enable,QString direction)
{
	Q_D(qSlicerCalculusReformatWidget);
	//ѡ\D4񴰿\DA SliceNodeSelector\B5\C4 NodeIDֵ
	//"vtkMRMLSliceNodeRed"
	// "vtkMRMLSliceNodeYellow"
	//"vtkMRMLSliceNodeGreen"
	//""

	d->SliceNodeSelector->setCurrentNodeID(direction);
	
	if (enable)
	{
		if (!d->ShowReformatWidgetToolButton->isChecked())//\D4\CA\D0\ED\B1\E4\D0\CE
			d->ShowReformatWidgetToolButton->toggle();
		if (!d->VisibilityCheckBox->isChecked())//\D4\CA\D0\ED\BFɼ\FB
			d->VisibilityCheckBox->toggle();
		
	}
	else
	{
		if (d->ShowReformatWidgetToolButton->isChecked())//\B9رձ\E4\D0\CE
			d->ShowReformatWidgetToolButton->toggle();
		if (d->VisibilityCheckBox->isChecked())//\B9رտɼ\FB
			d->VisibilityCheckBox->toggle();
	}

	
}

//------------------------------------------------------------------------------
/**
* @brief \D0\FDת
* @param QString direction,"LR" "PA" "IS"
* @param double value: \D0\FDת\BDǶ\C8
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::rotate(QString direction, double value)
{
	Q_D(qSlicerCalculusReformatWidget);
	if (direction == "LR")
	{
		d->LRSlider->setValue(value);
	}
	else if (direction == "PA")
	{
		d->PASlider->setValue(value);
	}
		
	else if (direction == "IS")
	{
		d->ISSlider->setValue(value);
	}
	getSliceRawData();

}
//------------------------------------------------------------------------------
/**
* @brief \CB\E6\BB\FA\BA\AF\CA\FD
* @param int min, int max
* @author liuzhaobang
* @date 2016-10-14
*/
int qSlicerCalculusReformatWidget::getRand(int min, int max)
{
	return min + qrand() % (max - min);
}
/**
* @brief \CB\E6\BB\FA\D0\FDת
* @param int min, int max
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::randRotate()
{
	Q_D(qSlicerCalculusReformatWidget);
	//\CB\E6\BB\FA\BA\AF\CA\FD\D6\D6\D7ӳ\F5ʼ\BB\AF\A3\AC\D4\CB\D0\D0һ\B4\CE\C8\ED\BC\FEֻ\B3\F5ʼ\BB\AF\D5\E2һ\B4Σ\AC\B1\A3֤\CB\E6\BB\FA\D0Բ\BB\D6ظ\B4\A1\A3
	QTime t;
	t = QTime::currentTime();
	qsrand(t.msec() + t.second() * 1000);
	int value = 0;
	m_lrValueList.clear();
	m_paValueList.clear();
	//\C8\CE\D2\E2\D0\FDת
	for (int i = 0; i < 16; i++)
	{
		value = getRand(-200, 200);
		m_lrValueList.append(value);
		
		value = getRand(-200, 200);
		m_paValueList.append(value);
		
	}
	//Test
	cout << "m_lrValueList:"<<endl;
	for (int i = 0; i < 16; i++)
	{
		cout << " "	<< m_lrValueList.at(i);

	}
	cout << endl;

	cout << "m_paValueList:" << endl;
	for (int i = 0; i < 16; i++)
	{
		cout << " " << m_paValueList.at(i);

	}
	cout << endl;


	m_lrTimerId = startTimer(1000);

	//m_paTimerId = startTimer(1000);
}
/**
* @brief \B6\A8ʱ\C6\F7\CA¼\FE
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::timerEvent(QTimerEvent *event)
{
	Q_D(qSlicerCalculusReformatWidget);
	if (event->timerId() == m_lrTimerId)
	{

		if (m_lrTimerCount < m_lrValueList.count())
		//if (m_lrTimerCount < 1)//\C1\D9ʱһ\B4\CE
		{
			rotate("LR", m_lrValueList[m_lrTimerCount]);
			m_lrTimerCount++;
			qDebug() << "qSlicerCalculusReformatWidget::timerEvent,m_lrTimerCount:" << m_lrTimerCount;
		}

		else
		{
			m_lrTimerCount = 0;
			killTimer(m_lrTimerId);
			m_lrTimerId=0;
			m_lrValueList.clear();

		}
	}
	else if (event->timerId() == m_paTimerId)
	{
		
		if (m_paTimerCount < m_paValueList.count())
		//if (m_paTimerCount < 1)
		{
			rotate("PA", m_paValueList[m_paTimerCount]);
			m_paTimerCount++;
			//qDebug() << "qSlicerCalculusReformatWidget::timerEvent,m_paTimerCount:" << m_paTimerCount;

		}

		else
		{
			m_paTimerCount = 0;
			killTimer(m_paTimerId);
			m_paTimerId = 0;
			m_paValueList.clear();
		}
	}
	//\B4\B9ֱ\B6\A8ʱ\C6\F7
	else if (event->timerId() == m_verticalTimerId)
	{
		double value = d->OffsetSlider->value();
		double singleStep = d->OffsetSlider->singleStep();
		double maximum = d->OffsetSlider->maximum();
		if ((value + singleStep) <= maximum)//\B3\AC\B3\F6\D7\EE\B4\F3ֵ
		{
			verticalAcqUi();//\B4\B9ֱ\B2ɼ\AF
			m_verticalTimerCount++;
			//qDebug() << "qSlicerCalculusReformatWidget::timerEvent,m_verticalTimerCount:" << m_verticalTimerCount;

		}

		else
		{
			m_verticalTimerCount = 0;
			killTimer(m_verticalTimerId);
			m_verticalTimerId = 0;
		}
	}
	//\C1\AC\D0\F8\B6\A8ʱ\C6\F7
	else if (event->timerId() == m_continueTimerId)
	{
		double value = d->OffsetSlider->value();
		double singleStep = d->OffsetSlider->singleStep();
		double maximum = d->OffsetSlider->maximum();
		if ((value + singleStep) <= maximum)//\B3\AC\B3\F6\D7\EE\B4\F3ֵ
		{
			verticalAcqUi();//\C8\D4Ȼ\D3\C3\D5\E2\B8\F6\BA\AF\CA\FD\A3\AC\B4\B9ֱ\B2ɼ\AF
			m_continueTimerCount++;
			//qDebug() << "qSlicerCalculusReformatWidget::timerEvent,m_verticalTimerCount:" << m_verticalTimerCount;

		}

		else
		{
			m_continueTimerCount = 0;
			killTimer(m_continueTimerId);
			m_continueTimerId = 0;
		}
	}
}

/**
* @brief \BB\F1ȡ\B5\B1\C7\D0Ƭ\CA\FD\BE\DD
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::getSliceRawData()
{
	Q_D(qSlicerCalculusReformatWidget);

	//\BB\F1\B5\C3\D7\EE\D0µĽڵ\E3\CA\FD\BE\DD
	vtkMRMLNode *mrmlNode = this->m_vtkMRMLScene->GetNodeByID("vtkMRMLSliceNodeRed");
	m_vtkMRMLSliceNodeRed = vtkMRMLSliceNode::SafeDownCast(mrmlNode);
	d->MRMLSliceLogic =
		this->logic()->GetMRMLApplicationLogic()->GetSliceLogic(m_vtkMRMLSliceNodeRed);

	/*d->MRMLSliceLogic =
		this->logic()->GetMRMLApplicationLogic()->GetSliceLogic(d->MRMLSliceNode);*/
	vtkMRMLSliceLayerLogic* sliceLayerLogic = d->MRMLSliceLogic->GetBackgroundLayer();
	vtkSmartPointer<vtkImageReslice> reslice = sliceLayerLogic->GetReslice();


	vtkNew<vtkTransform> transform;
	transform->SetMatrix(d->MRMLSliceNode->GetSliceToRAS());
	vtkSmartPointer<vtkMatrix4x4> resliceAxes;
	resliceAxes = transform->GetMatrix();
	


	
	m_stoneParamsHash.clear();
	//\BB\F1ȡ\B5\A5֡\C7\D0Ƭ\CA\FD\BE\DD
	if (m_calculusLogic)
	{
		m_stoneParamsHash = m_calculusLogic->acqSliceData(reslice.GetPointer(), m_vtkMRMLSliceNodeRed, m_vtkMRMLVolumeNode);
		if (m_stoneParamsHash.size() > 0)
		{
			emit newStoneParms(m_stoneParamsHash);
			//qDebug() << "m_stoneParamsHash" << " average=" << m_stoneParamsHash.value("average") << " AOD=" << m_stoneParamsHash.value("AOD") << " IOD=" << m_stoneParamsHash.value("IOD");
		}
	}


}

/**
* @brief \BB\F1ȡ\B4\B9ֱ\C7\D0Ƭ\CA\FD\BE\DD
* @author liuzhaobang
* @date 2016-10-14
*/
void qSlicerCalculusReformatWidget::getSliceVerticalRawData(double offset)
{
	Q_D(qSlicerCalculusReformatWidget);
	QString direction;
	QString nodeID;
	//Test===failed  \B2\BB֧\B3֡\B0=\A1\B1\A3\AC\B1\D8\D0\EBappend\BA\AF\CA\FD
	nodeID.append(d->SliceNodeSelector->currentNodeID());
	//qDebug() << "nodeID:" << nodeID;
	if (nodeID == "vtkMRMLSliceNodeRed")
	{
		direction = "Z";
	}
	else if (nodeID == "vtkMRMLSliceNodeYellow")
	{
		direction = "X";
	}
	else if (nodeID == "vtkMRMLSliceNodeGreen")
	{
		direction = "Y";
	}
	m_stoneParamsHash.clear();
	//\BB\F1ȡ\B5\A5֡\C7\D0Ƭ\CA\FD\BE\DD
	if (m_calculusLogic)
	{
		m_stoneParamsHash = m_calculusLogic->acqSliceVerticalData(m_vtkMRMLVolumeNode, offset, direction);
		if (m_stoneParamsHash.size() > 0)
		{
			emit newStoneParms(m_stoneParamsHash);
		}
	}

}
/**
* @brief \B4\B9ֱ\B2ɼ\AF
* @author liuzhaobang
* @date 2016-10-27
*/
void qSlicerCalculusReformatWidget::verticalAcq()
{
	Q_D(qSlicerCalculusReformatWidget);
	//\B4\B9ֱ\B2ɼ\AF\B3\F5ʼ\BB\AF
	double min = d->OffsetSlider->minimum();
	d->OffsetSlider->setValue(min);//\C9\E8\D6ôӵ\B1ǰ\B5\C4\D7\EEСֵ\BF\AAʼ

	m_verticalTimerId = startTimer(1000);//׼\B1\B8\BF\AA\C6\F4\B6\A8ʱ\C6\F7

}
/**
* @brief \C1\AC\D0\F8\B2ɼ\AF
* @author liuzhaobang
* @date 2016-10-27
*/
void qSlicerCalculusReformatWidget::continueAcq()
{
	Q_D(qSlicerCalculusReformatWidget);
	//\B4\B9ֱ\B2ɼ\AF\B3\F5ʼ\BB\AF
	double min = d->OffsetSlider->minimum();
	d->OffsetSlider->setValue(min);//\C9\E8\D6ôӵ\B1ǰ\B5\C4\D7\EEСֵ\BF\AAʼ

	m_continueTimerId = startTimer(1000);//׼\B1\B8\BF\AA\C6\F4\B6\A8ʱ\C6\F7

}

/**
* @brief ͨ\B9\FD\BD\E7\C3洹ֱ\B2ɼ\AF
* @author liuzhaobang
* @date 2016-10-27
*/
void qSlicerCalculusReformatWidget::verticalAcqUi()
{
	Q_D(qSlicerCalculusReformatWidget);
	double value = d->OffsetSlider->value();
	double singleStep = d->OffsetSlider->singleStep();
	d->OffsetSlider->setValue(value + singleStep);
}

/**
* @brief \B9ر\D5\CB\F9\D3\D0reformat\B4\B0\BF\DA
* @author liuzhaobang
* @date 2016-10-27
*/
void qSlicerCalculusReformatWidget::closeAllReformat()
{
	//ѡ\D4񴰿\DA SliceNodeSelector\B5\C4 NodeIDֵ
	//"vtkMRMLSliceNodeRed"
	// "vtkMRMLSliceNodeYellow"
	//"vtkMRMLSliceNodeGreen"
	enableReformat(false,"vtkMRMLSliceNodeRed");
	enableReformat(false, "vtkMRMLSliceNodeYellow");
	enableReformat(false, "vtkMRMLSliceNodeGreen");
}
/**
* @brief  \BB\F1ȡ\B4\B9ֱ\C7\D0\C3\E6\B2\CE\CA\FD
* @author liuzhaobang
* @date 2016-10-27
*/
void qSlicerCalculusReformatWidget::getVerticalStoneSlot(double value)
{
	//ֻ\D3п\AA\C6\F4\C1˴\B9ֱ\B2ɼ\AF\A3\AC\B2\C5\C4\DCִ\D0С\A3\CCر\F0\CAǷ\C0ֹ\B5\A5\B5\E3\B2ɼ\AF\B5\C4ʱ\BA\F2
	if (m_verticalTimerId != 0)
	{
		//\B2ɼ\AF\C7\D0\C3\E6\B2\CE\CA\FD
		getSliceVerticalRawData(value);
	}
	else if (m_continueTimerId != 0)
	{
		//\B2ɼ\AF\C7\D0\C3\E6\B2\CE\CA\FD
		getSliceRawData();
	}
}
/**
* @brief  \B2ɼ\AF֮ǰҪ\BC\A4\BB\EEReformat\B5\C4\CF\C2\C0\AD\BF\F2
* @author liuzhaobang
* @date 2016-10-27
*/
void qSlicerCalculusReformatWidget::enableReformatSelector()
{
	//\BC\AF\B3ɷ\C0ֹtest failed\A1\A3\BF\C9\D2\D4\C8\C3Reformat\B5\C4\CF\C2\C0\AD\BF\F2\D5\FD\B3\A3\CF\D4ʾ\A1\A3
	if (this->m_vtkMRMLScene != NULL)
		emit  mrmlSceneChanged(this->m_vtkMRMLScene);
}	
