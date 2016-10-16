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
// Qt includes
#include <QDebug>
#include <QMouseEvent>
#include <QToolTip>
// SlicerQt includes
#include "qSlicerCalculusModuleWidget.h"
#include "ui_qSlicerCalculusModuleWidget.h"
#include "qSlicerAbstractCoreModule.h"

#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>


// MRML includes
#include "vtkMRMLScene.h"

// Markups includes
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"

#include "vtkMRMLCropVolumeParametersNode.h"

#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"


// vtkSlicerCalculusLogic includes
#include "vtkSlicerCalculusLogic.h"
#include "qSlicerCalculusReformatWidget.h"
#include "vtkSlicerReformatLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCalculusModuleWidgetPrivate: public Ui_qSlicerCalculusModuleWidget
{
	Q_DECLARE_PUBLIC(qSlicerCalculusModuleWidget);
protected:
	qSlicerCalculusModuleWidget* const q_ptr;
public:
	qSlicerCalculusModuleWidgetPrivate(qSlicerCalculusModuleWidget& object);
	~qSlicerCalculusModuleWidgetPrivate();

  vtkSlicerCalculusLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerCalculusModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidgetPrivate::qSlicerCalculusModuleWidgetPrivate(qSlicerCalculusModuleWidget& object):q_ptr(&object)
{
}
qSlicerCalculusModuleWidgetPrivate::~qSlicerCalculusModuleWidgetPrivate()
{
}
vtkSlicerCalculusLogic* qSlicerCalculusModuleWidgetPrivate::logic() const
{
	Q_Q(const qSlicerCalculusModuleWidget);
	return vtkSlicerCalculusLogic::SafeDownCast(q->logic());
}
//-----------------------------------------------------------------------------
// qSlicerCalculusModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidget::qSlicerCalculusModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr(new qSlicerCalculusModuleWidgetPrivate(*this))
{


}

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidget::~qSlicerCalculusModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerCalculusModuleWidget::setup()
{
  Q_D(qSlicerCalculusModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // set up buttons connection
  connect(d->acqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onAcqStoneBtnClicked()));

  // set up input&output&markups connection
  connect(d->inputVolumeMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(onInputVolumeMRMLNodeChanged()));
  //connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->reformatWidget, SIGNAL(d->reformatWidget->mrmlSceneChanged(vtkMRMLScene*)));


  qSlicerAbstractCoreModule* reformatModule =
	  qSlicerCoreApplication::application()->moduleManager()->module("Reformat");
  if (reformatModule)
  {
	  vtkSlicerReformatLogic* reformatLogic =
		  vtkSlicerReformatLogic::SafeDownCast(reformatModule->logic());
	  //获取Reformat module的logic
	  d->reformatWidget->setReformatLogic(reformatLogic);
  }
  //传递本module的logic
  d->reformatWidget->setCalculusLogic(d->logic());
 
}

//-----------------------------------------------------------------------------
void qSlicerCalculusModuleWidget::setMRMLScene(vtkMRMLScene* scene)
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
void qSlicerCalculusModuleWidget::enter()
{
	// if there are already some
	// volumes or ROIs in the scene, they can be set up for use

	this->onInputVolumeMRMLNodeChanged();
	Q_D(qSlicerCalculusModuleWidget);
	//d->outputVolumeMRMLNodeComboBox->setEnabled(false);

	this->Superclass::enter();
}
void qSlicerCalculusModuleWidget::on_pushButton_clicked()
{
	QStringList namesList = qSlicerCoreApplication::application()->moduleManager()->modulesNames();
	QList<QString> names;

	for (int i = 0; i < namesList.size(); ++i)

	{
		names.append(namesList.at(i));
		qDebug() << " "<< namesList.at(i);
	}

}
void qSlicerCalculusModuleWidget::onAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	d->reformatWidget->enableReformat(true);//允许变形
	//设置VolumeMRMLNode
	d->reformatWidget->setVtkMRMLVolumeNode(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox->currentNode()));
	/*Q_D(qSlicerCalculusModuleWidget);
	vtkSmartPointer<vtkSlicerCalculusLogic> logic = d->logic();

	if (logic->acqSliceData(vtkMRMLScalarVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox->currentNode())))
	{
		qDebug() << "logic->AcqSliceData()";

	}*/

}
void qSlicerCalculusModuleWidget::onInputVolumeMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->inputVolumeMRMLNodeComboBox);
	updateAcqStoneButtonState();
	qDebug() << "onInputVolumeMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::updateAcqStoneButtonState()
{

	Q_D(qSlicerCalculusModuleWidget);
	if (d->inputVolumeMRMLNodeComboBox->currentNode())
	{
		d->acqStoneBtn->setToolTip("Input volume is required to do the segmentation.");
		QToolTip::showText(mapToGlobal(d->acqStoneBtn->pos()), QString("d->inputVolumeMRMLNodeComboBox"), (QWidget*)d->acqStoneBtn, QRect());
		d->acqStoneBtn->setEnabled(true);
	}
	


}
void qSlicerCalculusModuleWidget::onEndCloseEvent()
{
	Q_D(qSlicerCalculusModuleWidget);
	vtkSmartPointer<vtkSlicerCalculusLogic> logic = d->logic();
	//logic->reset(vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()), 0);
	//d->acqStoneBtn->setEnabled(true);
	cout << "close scene!" << endl;

}

