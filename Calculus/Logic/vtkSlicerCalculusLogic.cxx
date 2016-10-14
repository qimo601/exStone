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
//Qt includes
#include<qdebug.h>
// Calculus Logic includes
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerCalculusLogic);

//----------------------------------------------------------------------------
vtkSlicerCalculusLogic::vtkSlicerCalculusLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerCalculusLogic::~vtkSlicerCalculusLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerCalculusLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerCalculusLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerCalculusLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//设置当前体数据的logic
void vtkSlicerCalculusLogic::setVolumesLogic(vtkSlicerVolumesLogic* logic)
{
	this->volumesLogic = logic;
}
vtkSlicerVolumesLogic* vtkSlicerCalculusLogic::getVolumesLogic()
{
	return this->volumesLogic;
}
void vtkSlicerCalculusLogic::setCropVolumeLogic(vtkSlicerCropVolumeLogic* logic)
{
	this->cropVolumeLogic = logic;
}

vtkSlicerCropVolumeLogic* vtkSlicerCalculusLogic::getCropVolumeLogic()
{
	return this->cropVolumeLogic;
}

bool vtkSlicerCalculusLogic::acqSliceData(vtkMRMLSliceNode* input)
{
	//Set background box
	//Set pixels outside a box which is larger than the tightbox to be background
	MyBasic::Range3D bkgBox;
	MyBasic::Range3D imgBox;
	vtkImageData* orgimage = input->GetImageData();
	//尺寸长宽高
	int* dims = orgimage->GetDimensions();

	m_gData.wholeRange.col = dims[0];
	m_gData.wholeRange.row = dims[1];
	m_gData.wholeRange.sli = dims[2];
	qDebug() << "image dims:" << dims[0] << " " << dims[1] << " " << dims[2] << endl;

	//图像范围
	int extent[6];
	orgimage->GetExtent(extent);
	qDebug() << "image extent:" << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << " " << extent[4] << " " << extent[5];
	//每个像素的数量RGB
	int numberOfScalarComponents = orgimage->GetNumberOfScalarComponents();
	qDebug() << "image numberOfScalarComponents:" << numberOfScalarComponents;
	//图像原点
	double origin[3];
	orgimage->GetOrigin(origin);

	qDebug() << "image origin:" << origin[0] << " " << origin[1] << "" << origin[2];
	//像素间隔
	double spaceing[3];
	orgimage->GetSpacing(spaceing);
	qDebug() << "pixel space:" << spaceing[0] << " " << spaceing[1] << "" << spaceing[2];

	//
	////loadImage into gData
	//gData.loadImage(orgimage, imgBox);

	//Data3D<bool> mat_mask(gData.image.getSize(), true);

	//gData.seeds.set(gData.shifttightBox, UNKNOWN);

	//uchar* pixel = new uchar[m_gData.wholeRange.col *	m_gData.wholeRange.row * m_gData.wholeRange.sli]();
	//uchar* q = pixel;
	//for (int k = 0; k < dims[2]; k++)
	//{
	//	for (int j = 0; j < dims[1]; j++)
	//	{
	//		for (int i = 0; i < dims[0]; i++)
	//		{
	//			
	//				uchar* p = (uchar*)(orgimage->GetScalarPointer(i, j, k));
	//				q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *p;
	//				//q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *(p+1);
	//				//q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *(p+2);
	//		}

	//	}
	//}

	return true;
}
