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

#ifndef __qSlicerCalculusReformatWidget_h
#define __qSlicerCalculusReformatWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "vtkSlicerReformatLogic.h"
#include "vtkSlicerCalculusLogic.h"

// FooBar Widgets includes
#include "qSlicerCalculusModuleWidgetsExport.h"
#include<QTimerEvent>
#include "common.h"
class qSlicerCalculusReformatWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_Reformat
class Q_SLICER_MODULE_CALCULUS_WIDGETS_EXPORT
qSlicerCalculusReformatWidget : public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerCalculusReformatWidget(QWidget *parent=0);
  virtual ~qSlicerCalculusReformatWidget();
  virtual void timerEvent(QTimerEvent *event);
  enum OriginReferenceType {ONPLANE, INVOLUME};
  enum AxesReferenceType {axisX=0, axisY, axisZ};

  /// Utility function that sets the normal of the slice plane.
  void setSliceNormal(double x, double y, double z);
  //自定义 设置Reformat模块的logic
  void setReformatLogic(vtkSlicerReformatLogic* logic);
  //自定义 设置本module的Logic
  void setCalculusLogic(vtkSlicerCalculusLogic* logic);

  //自定义 获取本module的Logic
  vtkSlicerCalculusLogic* getCalculusLogic();
  void setVtkMRMLVolumeNode(vtkMRMLVolumeNode* node);
  vtkMRMLVolumeNode* getVtkMRMLVolumeNode();
  void setVtkMRMLSliceNodeRed(vtkMRMLSliceNode* node); 
	  vtkMRMLSliceNode* getVtkMRMLSliceNodeRed();
  void setVtkMRMLScene(vtkMRMLScene* scene);
	  vtkMRMLScene* getVtkMRMLScene();

	QHash<QString, double> m_stoneParamsHash;
	void setupSlot();
	/**
	* @brief  采集之前要激活Reformat的下拉框
	* @author liuzhaobang
	* @date 2016-10-27
	*/
	void enableReformatSelector();
protected:
  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);
  virtual void enter();
  vtkMRMLAbstractLogic* logic();
public slots:
  /// Set the position of the slice in world coordinates
  /// \sa setSliceOffsetValue
  void setWorldPosition(double* newWorldPosition);

  /// Set slice \a offset. Used to set a single value.
  /// \sa setWorldPosition
  void setSliceOffsetValue(double offset);

  /// Recenter the active node given its coordinates reference.
  void centerSliceNode();

  /// Set the normal of the slice plane. The origin of the plane is not
  /// changed. The \a normal is normalized before being set to the node.
  /// It resets the rotation sliders.
  /// \sa setWorldPosition.
  void setSliceNormal(double* normal);

  /// Set slice normal to the camera.
  void setNormalToCamera();

  /// Set the normal to a x axis
  void setNormalToAxisX();

  /// Set the normal to a y axis
  void setNormalToAxisY();

  /// Set the normal to a z axis
  void setNormalToAxisZ();

  void onEndCloseEvent();

  void onMRMLSceneChanged(vtkMRMLScene* scene);

  //允许当前的窗口变形
  void enableReformat(bool enable,QString direction);
  //旋转
  void rotate(QString direction, double value);
 
  //随机函数
  int getRand(int min, int max);
  //随机旋转
  void randRotate();
  //获取当切片数据
  void getSliceRawData();

  //获取垂直切片数据
  void getSliceVerticalRawData(double offset);

  /**
  * @brief 垂直采集
  * @author liuzhaobang
  * @date 2016-10-27
  */
  void verticalAcq();
  /**
  * @brief 通过界面垂直采集
  * @author liuzhaobang
  * @date 2016-10-27
  */
  void verticalAcqUi();
  /**
  * @brief 关闭所有reformat窗口
  * @author liuzhaobang
  * @date 2016-10-27
  */
  void closeAllReformat();
  /**
  * @brief  获取垂直切面参数
  * @author liuzhaobang
  * @date 2016-10-27
  */
  void getVerticalStoneSlot(double value);
signals:
  void newStoneParms(QHash<QString, double>);
protected slots:
  /// Triggered upon MRML transform node updates
  void onMRMLSliceNodeModified(vtkObject* caller);

  /// Set slice offset. Used when events will come is rapid succession.
  void onTrackSliceOffsetValueChanged(double offset);

  void onNodeSelected(vtkMRMLNode* node);
  void onSliceVisibilityChanged(bool visible);
  void onReformatWidgetVisibilityChanged(bool visible);
  void onLockReformatWidgetToCamera(bool lock);

  void onOriginCoordinateReferenceButtonPressed(int reference);

  void onSliceNormalToAxisChanged(AxesReferenceType axis);
  void onSliceOrientationChanged(const QString& orientation);
  void onSliderRotationChanged(double rotationX);

protected:
  QScopedPointer<qSlicerCalculusReformatWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerCalculusReformatWidget);
  Q_DISABLE_COPY(qSlicerCalculusReformatWidget);

private:
	int m_lrTimerId;//lr方向旋转，计时器
	int m_lrTimerCount;//lr执行次数
	int m_paTimerId;//pa方向旋转，计时器
	int m_paTimerCount;//pa执行次数

	int m_verticalTimerId;//垂直方向计时器
	int m_verticalTimerCount;//垂直方向执行次数
	QList<int> m_lrValueList;
	QList<int> m_paValueList;
  vtkSlicerReformatLogic* m_reformatLogic;
  vtkSlicerCalculusLogic* m_calculusLogic;
  vtkMRMLSliceNode* m_vtkMRMLSliceNodeRed;//当前Red的Node
  vtkMRMLVolumeNode* m_vtkMRMLVolumeNode;//当前分割后的数据
  vtkMRMLScene* m_vtkMRMLScene;//当前的场景
};

#endif
