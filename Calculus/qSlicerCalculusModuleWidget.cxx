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
#include   <iostream>   
using   namespace   std;
// SlicerQt includes
#include "qSlicerCalculusModuleWidget.h"
#include "ui_qSlicerCalculusModuleWidget.h"
#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCalculusModuleWidgetPrivate: public Ui_qSlicerCalculusModuleWidget
{
public:
  qSlicerCalculusModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCalculusModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidgetPrivate::qSlicerCalculusModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCalculusModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCalculusModuleWidget::qSlicerCalculusModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerCalculusModuleWidgetPrivate )
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