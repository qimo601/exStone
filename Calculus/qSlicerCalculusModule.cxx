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
#include <QtPlugin>
#include<qtextcodec.h>
// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>
#include "qSlicerAbstractCoreModule.h"

// Calculus Logic includes
#include <vtkSlicerCLIModuleLogic.h>
#include <vtkSlicerCropVolumeLogic.h>
#include <vtkSlicerVolumesLogic.h>

#include <vtkSlicerCalculusLogic.h>

// Calculus includes
#include "qSlicerCalculusModule.h"
#include "qSlicerCalculusModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerCalculusModule, qSlicerCalculusModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCalculusModulePrivate
{
public:
	qSlicerCalculusModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCalculusModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCalculusModulePrivate::qSlicerCalculusModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCalculusModule methods

//-----------------------------------------------------------------------------
qSlicerCalculusModule::qSlicerCalculusModule(QObject* _parent)
: Superclass(_parent)
, d_ptr(new qSlicerCalculusModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerCalculusModule::~qSlicerCalculusModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerCalculusModule::helpText() const
{
	return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerCalculusModule::acknowledgementText() const
{
	return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCalculusModule::contributors() const
{
	QStringList moduleContributors;
	moduleContributors << QString("John Doe (AnyWare Corp.)");
	return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerCalculusModule::icon() const
{
	return QIcon(":/Icons/Calculus.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerCalculusModule::categories() const
{
	return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCalculusModule::dependencies() const
{
	return QStringList();
}

//-----------------------------------------------------------------------------
//init interface
void qSlicerCalculusModule::setup()
{


	this->Superclass::setup();
	//get modulelogic
	vtkSlicerCalculusLogic* moduleLogic = vtkSlicerCalculusLogic::SafeDownCast(this->logic());
	
	QStringList namesList = qSlicerCoreApplication::application()->moduleManager()->modulesNames();

	//获取其他module的logic
	//获取体数据module，通过module的name获取
	//qSlicerAbstractCoreModule* volumesModule =
	//	qSlicerCoreApplication::application()->moduleManager()->module("Volumes");
	//if (volumesModule)
	//{
	//	//获取volumesModule的logic
	//	vtkSlicerVolumesLogic* volumesLogic = vtkSlicerVolumesLogic::SafeDownCast(volumesModule->logic());
	//	//设置当前module的体数据业务
	//	moduleLogic->setVolumesLogic(volumesLogic);
	//}

	////获取剪切后体数据的logic
	//qSlicerAbstractCoreModule* cropVolumeModule =
	//	qSlicerCoreApplication::application()->moduleManager()->module("CropVolume");
	//if (cropVolumeModule)
	//{
	//	vtkSlicerCropVolumeLogic* cropVolumeLogic =
	//		vtkSlicerCropVolumeLogic::SafeDownCast(cropVolumeModule->logic());
	//	moduleLogic->setCropVolumeLogic(cropVolumeLogic);
	//}



}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerCalculusModule
::createWidgetRepresentation()
{
	return new qSlicerCalculusModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCalculusModule::createLogic()
{
	return vtkSlicerCalculusLogic::New();
}
