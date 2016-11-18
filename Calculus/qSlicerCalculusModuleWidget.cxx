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

//--------------add by zhushanshan-------------------------------
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLLabelMapVolumeNode.h"
//------------------------------
#include "vtkMRMLCropVolumeParametersNode.h"

#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"

// vtkSlicerCalculusLogic includes
#include "vtkSlicerCalculusLogic.h"
//-----------------------
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QtGui>

#include <Qtcore/qstring.h>
#include <Qtcore/QFile>
#include <stdexcept>

#include <QTableWidget>  
#include <QTableWidgetItem>
using namespace std;
class QAxObject;
//-------------------add end--------------------------------------
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
	m_enableReformat = false;//reformat window,defalut false
	qDebug() << "void qSlicerCalculusModuleWidget::constuctor class.";
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
  connect(d->x_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onX_VerticalAcqStoneBtnClicked()));
  connect(d->y_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onY_VerticalAcqStoneBtnClicked()));
  connect(d->z_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onZ_VerticalAcqStoneBtnClicked()));
  
  // set up input&output&markups connection
  connect(d->inputVolumeMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(onInputVolumeMRMLNodeChanged()));
  //connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->reformatWidget, SIGNAL(d->reformatWidget->mrmlSceneChanged(vtkMRMLScene*)));

  //--------------add by zhushanshan----------------------------
  connect(d->inputVolumeMRMLNodeComboBox_2, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(InputVolumeMRMLNodeChanged()));

  connect(d->markupsMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onMarkupsMRMLNodeChanged()));

  connect(d->generateButton, SIGNAL(clicked()), this, SLOT(generateClicked()));

  connect(d->saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));
  connect(d->saveButton_2, SIGNAL(clicked()), this, SLOT(saveClicked_2()));

  connect(d->clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClicked()));
  connect(d->clearButton_2, SIGNAL(clicked()), this, SLOT(clearButtonClicked_2()));

  //--------------------and end-------------------------

  qSlicerAbstractCoreModule* reformatModule =
	  qSlicerCoreApplication::application()->moduleManager()->module("Reformat");
  if (reformatModule)
  {
	  vtkSlicerReformatLogic* reformatLogic =
		  vtkSlicerReformatLogic::SafeDownCast(reformatModule->logic());
	  //get Reformat module logic
	  d->reformatWidget->setReformatLogic(reformatLogic);
  }
  //set module logic
  d->reformatWidget->setCalculusLogic(d->logic());
  connect(d->reformatWidget, SIGNAL(newStoneParms(QHash<QString,double>)), this, SLOT(addStoneParmsSlot(QHash<QString,double>)));



  qDebug() << "void qSlicerCalculusModuleWidget::setup()";
 
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
//-----------------------------------------------------------------------------
//get params from ui
void qSlicerCalculusModuleWidget::getParamsFromUi()
{
	Q_D(qSlicerCalculusModuleWidget);
	//get three params
	vtkSlicerCalculusLogic::s_sliceThick = d->sliceThickSpinBox->value();
	vtkSlicerCalculusLogic::s_uWater = d->uWaterSpinBox->value();
	vtkSlicerCalculusLogic::s_materialThick = d->materialThickSpinBox->value();

	vtkMRMLNode *mrmlNode = this->mrmlScene()->GetNodeByID("vtkMRMLSliceNodeRed");

	//no use
	d->reformatWidget->setVtkMRMLVolumeNode(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox->currentNode()));
	//no use
	d->reformatWidget->setVtkMRMLSliceNodeRed(vtkMRMLSliceNode::SafeDownCast(mrmlNode));
	//set VolumeMRMLScene
	d->reformatWidget->setVtkMRMLScene(this->mrmlScene());
	

}

//open reforamt window
void qSlicerCalculusModuleWidget::on_openBtn_clicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	//make sure the m_enableReformat  only once
	if (!m_enableReformat)
	{
		d->reformatWidget->setupSlot();
		m_enableReformat = true;

		d->reformatWidget->enableReformatSelector();

	}
}
void qSlicerCalculusModuleWidget::onAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	getParamsFromUi();

	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");
	d->reformatWidget->randRotate();


}

void qSlicerCalculusModuleWidget::onX_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();

	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeYellow");
	d->reformatWidget->verticalAcq();

}
void qSlicerCalculusModuleWidget::onY_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();

	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeGreen");
	d->reformatWidget->verticalAcq();

}

void qSlicerCalculusModuleWidget::onZ_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();

	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");
	d->reformatWidget->verticalAcq();

}
void qSlicerCalculusModuleWidget::onInputVolumeMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->inputVolumeMRMLNodeComboBox);
	updateAcqStoneButtonState();
	getParamsFromUi();

	if (d->inputVolumeMRMLNodeComboBox->currentNodeID() != "")
	{
		d->acqStoneBtn->setEnabled(true);
		d->x_verticalAcqStoneBtn->setEnabled(true);
		d->y_verticalAcqStoneBtn->setEnabled(true);
		d->z_verticalAcqStoneBtn->setEnabled(true);
	}
	else
	{
		d->acqStoneBtn->setEnabled(false);
		d->x_verticalAcqStoneBtn->setEnabled(false);
		d->y_verticalAcqStoneBtn->setEnabled(false);
		d->z_verticalAcqStoneBtn->setEnabled(false);
	}

	//qDebug() << "onInputVolumeMRMLNodeChanged" << endl;
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
	//----------------------add by zhushanshan---------------------
	d->generateButton->setEnabled(true);
	d->markupsMRMLNodeComboBox->setEnabled(true);
//----------------------add end---------------------
	//logic->reset(vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()), 0);
	//d->acqStoneBtn->setEnabled(true);
	cout << "close scene!" << endl;

}
//----------------------add by zhushanshan---------------------
void qSlicerCalculusModuleWidget::InputVolumeMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->inputVolumeMRMLNodeComboBox);
	updategenerateButtonState();
	qDebug() << "InputVolumeMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::onMarkupsMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->markupsMRMLNodeComboBox);
	qDebug() << "onMarkupsMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::updategenerateButtonState()
{

	Q_D(qSlicerCalculusModuleWidget);
	if (d->inputVolumeMRMLNodeComboBox->currentNode())
	{
		d->generateButton->setToolTip("Input volume is required to do the segmentation.");
		d->generateButton->setEnabled(true);
	}
}
void qSlicerCalculusModuleWidget::generateClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	vtkSlicerCalculusLogic *logic = d->logic();
	QHash<QString, double> paramHash;
	paramHash = logic->aqcCircleData(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox_2->currentNode()), vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()));

	addTableWidgetRow(paramHash,d->tableblock);
	d->saveButton->setEnabled(true);
	
}
void qSlicerCalculusModuleWidget::addStoneParmsSlot(QHash<QString,double> hash)
{
	Q_D(qSlicerCalculusModuleWidget);
	addTableWidgetRow(hash,d->tableblock_2);
	d->saveButton_2->setEnabled(true);
}
//tableWidget add row
void qSlicerCalculusModuleWidget::addTableWidgetRow(QHash<QString,double> paramHash,QTableWidget* widget)
{
	int counts = widget->rowCount();
	widget->insertRow(counts);
	QString a, b, c,e,f;
	a = QString::number(paramHash.value("max"));
	b = QString::number(paramHash.value("min"));
	c = QString::number(paramHash.value("average"));//double to qstring
	e = QString::number(paramHash.value("AOD"));
	f = QString::number(paramHash.value("IOD"));
	qDebug() << b << endl;
	widget->setItem(counts, 0, new QTableWidgetItem(a));
	widget->setItem(counts, 1, new QTableWidgetItem(b));
	widget->setItem(counts, 2, new QTableWidgetItem(c));
	widget->setItem(counts, 3, new QTableWidgetItem(e));
	widget->setItem(counts, 4, new QTableWidgetItem(f));
	/*qDebug() << paramHash.value("max") << endl;
	*/
}
////save tablewidget to excel
void qSlicerCalculusModuleWidget::saveClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	ExcelExportHelper excel;
	for (int i = 0; i < (d->tableblock->rowCount()); i++)
	{
		for (int j = 0; j < (d->tableblock->columnCount() ); j++)
		{
			QString str = d->tableblock->item((i ), (j ))->text();
			qDebug() << str << endl;
			excel.SetCellValue(i+1, j+1, str);
		}
	}
	//const QString fileName = "E:\\kaka14.xlsx";
	//excel.SaveAs(fileName);
}
////save tablewidget to excel
void qSlicerCalculusModuleWidget::saveClicked_2()
{
	Q_D(qSlicerCalculusModuleWidget);
	ExcelExportHelper excel;
	for (int i = 0; i < (d->tableblock_2->rowCount()); i++)
	{
		for (int j = 0; j < (d->tableblock_2->columnCount()); j++)
		{
			QString str = d->tableblock_2->item((i), (j))->text();
			qDebug() << str << endl;
			excel.SetCellValue(i+1, j+1+8, str);
		}
	}
	//const QString fileName = "E:\\kaka14.xlsx";
	//excel.SaveAs(fileName);
}
void qSlicerCalculusModuleWidget::clearButtonClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	int counts = d->tableblock->rowCount();
	for (int i = 0; i < counts; i++)
	{
		d->tableblock->removeRow(d->tableblock->rowCount()-1);
	}
}
void qSlicerCalculusModuleWidget::clearButtonClicked_2()
{
	Q_D(qSlicerCalculusModuleWidget);
	d->reformatWidget->setupSlot();

	int counts = d->tableblock_2->rowCount();
	for (int i = 0; i < counts; i++)
	{
		d->tableblock_2->removeRow(d->tableblock_2->rowCount()-1);
	}
}
ExcelExportHelper::ExcelExportHelper(bool closeExcelOnExit)
{
	m_closeExcelOnExit = closeExcelOnExit;
	m_excelApplication = nullptr;
	m_sheet = nullptr;
	m_sheets = nullptr;
	m_workbook = nullptr;
	m_workbooks = nullptr;
	m_excelApplication = nullptr;

	m_excelApplication = new QAxObject("Excel.Application", 0);

	if (m_excelApplication == nullptr)
		throw invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");

	m_excelApplication->dynamicCall("SetVisible(bool)", false); // hide excel
	m_excelApplication->setProperty("DisplayAlerts", 1); // disable alerts

	m_workbooks = m_excelApplication->querySubObject("Workbooks");
	m_workbook = m_workbooks->querySubObject("Add");
	m_sheets = m_workbook->querySubObject("Worksheets");
	m_sheet = m_sheets->querySubObject("Add");
}

void ExcelExportHelper::SetCellValue(int lineIndex, int columnIndex, const QString& value)
{
	QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", lineIndex, columnIndex);
	cell->setProperty("Value", value);
	delete cell;
}
ExcelExportHelper::~ExcelExportHelper()
{
	if (m_excelApplication != nullptr)
	{
		if (!m_closeExcelOnExit)
		{
			m_excelApplication->setProperty("DisplayAlerts", 1);
			m_excelApplication->dynamicCall("SetVisible(bool)", true);
		}

		if (m_workbook != nullptr && m_closeExcelOnExit)
		{
			m_workbook->dynamicCall("Close (Boolean)", true);
			m_excelApplication->dynamicCall("Quit (void)");
		}
	}

	delete m_sheet;
	delete m_sheets;
	delete m_workbook;
	delete m_workbooks;
	delete m_excelApplication;
}
//----------------------add end ---------------------