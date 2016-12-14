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
#include <QMessageBox>
#include "qt_windows.h"
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
  //屏蔽精简版
  //d->CTKCollapsibleButton_3->setVisible(false);
 /* d->singleAcqStoneBtn->setVisible(false);
  d->x_verticalAcqStoneBtn->setVisible(false);
  d->y_verticalAcqStoneBtn->setVisible(false);
  d->z_verticalAcqStoneBtn->setVisible(false);
  d->continueAcqStoneBtn->setVisible(false);
  d->generateButton_2->setVisible(false);*/

  // set up buttons connection
  connect(d->acqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onAcqStoneBtnClicked()));
  connect(d->x_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onX_VerticalAcqStoneBtnClicked()));
  connect(d->singleAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onSingleAcqStoneBtnClicked()));
  connect(d->y_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onY_VerticalAcqStoneBtnClicked()));
  connect(d->z_verticalAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(onZ_VerticalAcqStoneBtnClicked()));
  connect(d->continueAcqStoneBtn, SIGNAL(clicked()),
	  this, SLOT(oncontinueAcqStoneBtnClicked()));

  // set up input&output&markups connection
  connect(d->inputVolumeMRMLNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
	  this, SLOT(onInputVolumeMRMLNodeChanged()));
  //connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->reformatWidget, SIGNAL(d->reformatWidget->mrmlSceneChanged(vtkMRMLScene*)));

  //--------------add by zhushanshan----------------------------
  connect(d->inputVolumeMRMLNodeComboBox_2, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(InputVolumeMRMLNodeChanged()));

  connect(d->generateButton, SIGNAL(clicked()), this, SLOT(generateClicked()));
  connect(d->generateButton_2, SIGNAL(clicked()), this, SLOT(generateClicked_2()));
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

	bool ok;
	QString text = QInputDialog::getText(this, tr("Input your password."),
		tr("Password:"), QLineEdit::Normal,"", &ok);
	if (ok && !text.isEmpty())
		qDebug() << "input:" << text;
	if (!verificationPassword(text))
	{
		QMessageBox msgBox;
		msgBox.setText("Error");
		QString content1 = "Incorrect Password or license is out of date";
		QString content;
		content.append(content1);
		msgBox.setInformativeText(content);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();

		return;
	}


	//make sure the m_enableReformat  only once
	if (!m_enableReformat)
	{
		d->reformatWidget->setupSlot();
		m_enableReformat = true;

		d->reformatWidget->enableReformatSelector();

	}

}
//set new password
void qSlicerCalculusModuleWidget::setPassword()
{
	Q_D(qSlicerCalculusModuleWidget);
	QFile file("key");
	if (!file.open(QIODevice::WriteOnly))
		return;

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	QString description = "sibet liuzhaobang";
	QString password = "ct";
	QDate date(2016, 12, 30);
	QDateTime dateTime(date);
	
	QByteArray by1;
	by1.append(password);
	QCryptographicHash *hash = new QCryptographicHash(QCryptographicHash::Md5);
	hash->addData(by1);
	QByteArray by2 = hash->result();
	 
	out << description << by2 << dateTime;
	file.close();
}
//read this password
QString qSlicerCalculusModuleWidget::readPassword()
{
	Q_D(qSlicerCalculusModuleWidget);
	QFile file("key");
	if (!file.open(QIODevice::ReadOnly))
		return 0;

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_8);
	QString description;
	QString password;
	QDateTime datetime;
	QByteArray by1;
	in >> description >> by1 >> datetime;

	qDebug() << "description:" << description << "password:" << password << "DateTime:" << datetime;
	file.close();

	return password;
}
//verification password
bool qSlicerCalculusModuleWidget::verificationPassword(QString password)
{
	Q_D(qSlicerCalculusModuleWidget);
	//make sure the m_enableReformat  only once
	if (password == "ct")
	{
		QDateTime dateTime1;
		QDateTime dateTime2 = dateTime1.currentDateTime();
		QDate date(2016,12,30);
		QDateTime dateTime3(date);
		if (dateTime2 < dateTime3)
			return true;
		else
			return false;
	}
	else
		return false;


}
void qSlicerCalculusModuleWidget::onAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	getParamsFromUi();

	d->fileNameComboBox->setCurrentIndex(0);
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");
	d->reformatWidget->randRotate();


}

void qSlicerCalculusModuleWidget::onSingleAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	getParamsFromUi();
	d->fileNameComboBox->setCurrentIndex(1);
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");
	d->reformatWidget->getSliceRawData();


}
void qSlicerCalculusModuleWidget::onX_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();
	d->fileNameComboBox->setCurrentIndex(2);
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeYellow");
	d->reformatWidget->verticalAcq();

}
void qSlicerCalculusModuleWidget::onY_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();
	d->fileNameComboBox->setCurrentIndex(3);
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeGreen");
	d->reformatWidget->verticalAcq();

}

void qSlicerCalculusModuleWidget::onZ_VerticalAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();
	d->fileNameComboBox->setCurrentIndex(4);
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");
	d->reformatWidget->verticalAcq();

}
void qSlicerCalculusModuleWidget::oncontinueAcqStoneBtnClicked()
{
	Q_D(qSlicerCalculusModuleWidget);

	getParamsFromUi();
	d->fileNameComboBox->setCurrentIndex(5);
	d->reformatWidget->closeAllReformat();
	d->reformatWidget->enableReformat(true, "vtkMRMLSliceNodeRed");
	d->reformatWidget->continueAcq();

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
		d->singleAcqStoneBtn->setEnabled(true);
		d->x_verticalAcqStoneBtn->setEnabled(true);
		d->y_verticalAcqStoneBtn->setEnabled(true);
		d->z_verticalAcqStoneBtn->setEnabled(true);
		d->continueAcqStoneBtn->setEnabled(true);
	}
	else
	{
		d->acqStoneBtn->setEnabled(false);
		d->singleAcqStoneBtn->setEnabled(false);
		d->x_verticalAcqStoneBtn->setEnabled(false);
		d->y_verticalAcqStoneBtn->setEnabled(false);
		d->z_verticalAcqStoneBtn->setEnabled(false);
		d->continueAcqStoneBtn->setEnabled(false);
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
//----------------------add end---------------------
	//logic->reset(vtkMRMLMarkupsFiducialNode::SafeDownCast(d->markupsMRMLNodeComboBox->currentNode()), 0);
	//d->acqStoneBtn->setEnabled(true);
	cout << "close scene!" << endl;

}
//----------------------add by zhushanshan---------------------
void qSlicerCalculusModuleWidget::InputVolumeMRMLNodeChanged()
{
	Q_D(qSlicerCalculusModuleWidget);
	Q_ASSERT(d->inputVolumeMRMLNodeComboBox_2);
	updategenerateButtonState();
	qDebug() << "InputVolumeMRMLNodeChanged" << endl;
}
void qSlicerCalculusModuleWidget::updategenerateButtonState()
{

	Q_D(qSlicerCalculusModuleWidget);
	if (d->inputVolumeMRMLNodeComboBox_2->currentNodeID() != "")
	{
		d->generateButton->setToolTip("Input volume is required to do the segmentation.");
		d->generateButton->setEnabled(true);
		d->generateButton_2->setEnabled(true);
	}
	else
	{
		d->generateButton->setEnabled(false);
		d->generateButton_2->setEnabled(false);

	}
}
void qSlicerCalculusModuleWidget::generateClicked()
{
	Q_D(qSlicerCalculusModuleWidget);
	d->fileNameComboBox->setCurrentIndex(6);
	vtkSlicerCalculusLogic *logic = d->logic();
	QHash<QString, double> paramHash;
	paramHash = logic->aqcCircleData(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox_2->currentNode()));

	addTableWidgetRow(paramHash,d->tableblock);
	d->saveButton->setEnabled(true);
	
}
void qSlicerCalculusModuleWidget::generateClicked_2()
{
	Q_D(qSlicerCalculusModuleWidget);
	d->fileNameComboBox->setCurrentIndex(7);
	vtkSlicerCalculusLogic *logic = d->logic();
	QHash<QString, double> paramHash;
	paramHash = logic->aqcCircleData(vtkMRMLVolumeNode::SafeDownCast(d->inputVolumeMRMLNodeComboBox_2->currentNode()));

	addTableWidgetRow(paramHash, d->tableblock);
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
	//Test failed
	a.setNum(paramHash.value("max"));
	b.setNum(paramHash.value("min"));
	c.setNum(paramHash.value("average"));//double to qstring
	e.setNum(paramHash.value("AOD"));
	f.setNum(paramHash.value("IOD"));
	//a = QString::number(paramHash.value("max"));
	//b = QString::number(paramHash.value("min"));
	//c = QString::number(paramHash.value("average"));//double to qstring
	//e = QString::number(paramHash.value("AOD"));
	//f = QString::number(paramHash.value("IOD"));
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
	int counts = d->tableblock->rowCount();
	if (counts == 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Error");
		QString content1 = "No data need to save.";
		QString content;
		content.append(content1);
		msgBox.setInformativeText(content);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
	}
	else
	{
		QString fileNameType = d->fileNameComboBox->currentText();
		ExcelExportHelper excel(fileNameType);
		for (int i = 0; i < (d->tableblock->rowCount()); i++)
		{
			QString line;
			for (int j = 0; j < (d->tableblock->columnCount() ); j++)
			{
				QString str = d->tableblock->item((i ), (j ))->text();
				line = line + str + ",";
			}
			excel.SetCellValue(line);
		}
	
		//const QString fileName = "E:\\kaka14.xlsx";
		//excel.SaveAs(fileName);
	}

}
////save tablewidget to excel
void qSlicerCalculusModuleWidget::saveClicked_2()
{
	Q_D(qSlicerCalculusModuleWidget);
	int counts = d->tableblock_2->rowCount();

	if (counts == 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Error");
		QString content1 = "No data need to save.";
		QString content;
		content.append(content1);
		msgBox.setInformativeText(content);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
	}
	else
	{
		QString fileNameType = d->fileNameComboBox->currentText();
		ExcelExportHelper excel(fileNameType);
		for (int i = 0; i < (d->tableblock_2->rowCount()); i++)
		{
			QString line;
			for (int j = 0; j < (d->tableblock_2->columnCount()); j++)
			{
				QString str = d->tableblock_2->item(i, j)->text();
				line = line + str + ",";
			}
			excel.SetCellValue(line);
		}

		//const QString fileName = "E:\\kaka14.xlsx";
		//excel.SaveAs(fileName);
	}
	
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

	int counts = d->tableblock_2->rowCount();
	for (int i = 0; i < counts; i++)
	{
		d->tableblock_2->removeRow(d->tableblock_2->rowCount()-1);
	}
}
ExcelExportHelper::ExcelExportHelper(QString type)
{
	QDateTime dateTime;
	QDateTime dateTime1 =  dateTime.currentDateTime();
	m_fileName = dateTime1.date().toString("yyyy-MM-dd") + "_" + dateTime1.time().toString("HHmmss");
	
	QString path = "D:\\SlicerExcelData";
	QDir dir(path);
	if (!dir.exists())
	{
		dir.mkdir("D:\\SlicerExcelData");
	}
	QString m_path1;
	if (type == "")
	{
		m_path1 = "D:\\SlicerExcelData\\%1.csv";
		m_path.append(m_path1.arg(m_fileName));
	}
	else
	{
		m_path1 = "D:\\SlicerExcelData\\%1_%2.csv";
		m_path.append(m_path1.arg(type).arg(m_fileName));
	}
	m_file = new QFile(m_path);
	Open(m_path);

	qDebug() << "Test m_fileName:" << m_fileName<< "Test m_path:" << m_path;
}
ExcelExportHelper::~ExcelExportHelper()
{
	m_file->close();
	delete m_file;
	QMessageBox msgBox;
	msgBox.setText("Excel file");
	QString content1 = "parmas have saved here, %1";
	QString content;
	content.append(content1.arg(m_path));
	msgBox.setInformativeText(content);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Save | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();
	QString fileName;
	switch (ret) {
	case QMessageBox::Save:
	{
							  // Save was clicked
							  QFileDialog *fd = new QFileDialog();
							  fd->setWindowTitle("Save As");
							  fd->setAcceptMode(QFileDialog::AcceptSave);
							  fd->setFileMode(QFileDialog::AnyFile); 
							  //fd->setViewMode(QFileDialog::Detail);
							  fd->setGeometry(10, 30, 600, 400);
							  fd->setDirectory("D:\\SlicerExcelData");
							  QStringList nameFilters;
							  nameFilters << "Excel files (*.csv *.CSV)";
							  fd->setNameFilters(nameFilters);

							  QStringList fileNamesList;
							  if (fd->exec() == QDialog::Accepted) {
								  fileNamesList = fd->selectedFiles();
							  }
							  else
								  break;


							  fileName = fileNamesList.at(0).toLocal8Bit().constData();

							  //!--- 复制文件  
							  fileName.append(".csv");
							  bool copy_error = m_file->copy(m_path, fileName);
							  QString con;
							  if (copy_error)
							  {
								  con = "saved successed.";
							  }
							  else
							  {
								  con = "saved failed.";
							  }
							  QMessageBox msg;
							  msg.setText("reuslt:");
							  msg.setInformativeText(con);
							  msg.setStandardButtons(QMessageBox::Ok);
							  msg.setDefaultButton(QMessageBox::Ok);
							  msg.exec();

							  delete fd;
							  break;
	}
	case QMessageBox::Discard:
		// Don't Save was clicked
		break;
	case QMessageBox::Cancel:
		// Cancel was clicked
		break;
	default:
		// should never be reached
		break;
	}

	

}
void ExcelExportHelper::SetCellValue( QString value)
{
	QTextStream out(m_file);
	out << value << "\n";
	qDebug() << "Test Cell Value:" << value;
}
void ExcelExportHelper::Open(QString fileName)
{
	if (!m_file->open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Failed,ExcelExportHelper::open " << m_path;
	}

	QTextStream out(m_file);
	QString value = "MAX,";
	value += "MIN,";
	value += "AVERAGE,";
	value += "AOD,";
	value += "IOD";
	out << value << "\n";
}

//----------------------add end ---------------------