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
//---------------朱珊珊添加
//excel有关头文件
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <Qtcore/qstring.h>
//与窗口有关的头文件
#include <QDialog>
#include <QtCore/QVariant>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QLayout>
#include <QtGui/QApplication>  
//与tableWidget有关的类
#include <QTableWidget>  
#include <QTableWidgetItem>
#include <QDialog>
#include <QAction>
#include <QApplication>
class QLabel;
class QPushButton;
//--------------------添加结束-----------------
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
  //在tableWidget里添加一行参数
  void addTableWidgetRow(QHash<QString, double> paramHash, QTableWidget* widget);
  //-----------------------------------------------------------------------------
  //获取界面参数
  void getParamsFromUi();
signals:
  void clicked();

public slots:
//CHEN
/// Update the selection node from the combo box
void onInputVolumeMRMLNodeChanged();
void updateAcqStoneButtonState();

/// Display property button slots
///acquire the urinary calculi parameters
void onAcqStoneBtnClicked();

//结石垂直X轴方向采集参数
void onX_VerticalAcqStoneBtnClicked();
//结石垂直Y轴方向采集参数
void onY_VerticalAcqStoneBtnClicked();
//结石垂直Z轴方向采集参数
void onZ_VerticalAcqStoneBtnClicked();
/**
* @brief 场景关闭事件
* @author liuzhaobang
* @date 2016-10-14
*/
void onEndCloseEvent();
void addStoneParmsSlot(QHash<QString, double> hash);
//---------------朱珊珊添加
void InputVolumeMRMLNodeChanged();
void onMarkupsMRMLNodeChanged();
void updategenerateButtonState();
void saveClicked(); //点击保存按钮，发出保存信号
void saveClicked_2(); //点击保存按钮，发出保存信号
void generateClicked();//点击生成按钮，将文档中数据导入QTablewidget
void clearButtonClicked();//清空表格
void clearButtonClicked_2();

//---------------添加结束
protected:
  QScopedPointer<qSlicerCalculusModuleWidgetPrivate> d_ptr;

  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);
  virtual void enter();
private:
  Q_DECLARE_PRIVATE(qSlicerCalculusModuleWidget);
  Q_DISABLE_COPY(qSlicerCalculusModuleWidget);
};

//---------------朱珊珊添加
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


//---------------添加结束
#endif
