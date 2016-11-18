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
#include "Logic/common.h"
//excel
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
//GUI
#include <QDialog>
#include <QtCore/QVariant>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QLayout>
#include <QtGui/QApplication>  
//tableWidget
#include <QTableWidget>  
#include <QTableWidgetItem>
#include <QDialog>
#include <QAction>
#include <QApplication>
class QLabel;
class QPushButton;
//--------------------add end-----------------
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
  //tableWidget add one row
  void addTableWidgetRow(QHash<QString, double> paramHash, QTableWidget* widget);
  //-----------------------------------------------------------------------------
  //get ui params
  void getParamsFromUi();
signals:
  void clicked();

public slots:
//CHEN
/// Update the selection node from the combo box
void onInputVolumeMRMLNodeChanged();
void updateAcqStoneButtonState();
//open Reformat window
void on_openBtn_clicked();
/// Display property button slots
///acquire the urinary calculi parameters
void onAcqStoneBtnClicked();

//stone vertical x axial
void onX_VerticalAcqStoneBtnClicked();
//stone vertical y axial
void onY_VerticalAcqStoneBtnClicked();
//stone vertical z axial
void onZ_VerticalAcqStoneBtnClicked();
//
void onEndCloseEvent();
void addStoneParmsSlot(QHash<QString, double> hash);
//---------------add by zhushanshan
void InputVolumeMRMLNodeChanged();
void onMarkupsMRMLNodeChanged();
void updategenerateButtonState();
void saveClicked(); //save excel bed acq
void saveClicked_2(); //save excel auto acq
void generateClicked();//params to QTablewidget
void clearButtonClicked();//clear table
void clearButtonClicked_2();


//---------------add end---
protected:
  QScopedPointer<qSlicerCalculusModuleWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);
  virtual void enter();
private:
  Q_DECLARE_PRIVATE(qSlicerCalculusModuleWidget);
  Q_DISABLE_COPY(qSlicerCalculusModuleWidget);
  bool m_enableReformat;//Reformat window use
};

//---------------by zhushanshan
class ExcelExportHelper
{
public:
	ExcelExportHelper(const ExcelExportHelper& other) = delete;
	ExcelExportHelper& operator=(const ExcelExportHelper& other) = delete;

	ExcelExportHelper(bool closeExcelOnExit = false);
	void SetCellValue(int lineIndex, int columnIndex, const QString& value);
	void SaveAs(const QString& fileName);

	~ExcelExportHelper();

private:
	QAxObject* m_excelApplication;
	QAxObject* m_workbooks;
	QAxObject* m_workbook;
	QAxObject* m_sheets;
	QAxObject* m_sheet;
	bool m_closeExcelOnExit;

	
};


//---------------add end---
#endif
