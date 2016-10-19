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
//----------------朱珊珊添加---------------
#include "vtkSlicerCalculusLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsLogic.h"
// STD includes
#include <cassert>
//-------------
#include <vtkImageData.h>
#include <cassert>
#include <vector>
#include <vtkIOStream.h>
#include<sstream> 
#include<qstring.h>
#include <QDebug>
#include <fstream>

#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>
#include <QtGui>

#include <Qtcore/qstring.h>
#include <Qtcore/QFile>
#include <stdexcept>

#include <QHash>
#include "vtkSlicerCalculusLogic.h"
using namespace std;
class QAxObject;
//----------------------------添加结束----------------------------

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

QHash<QString, double> vtkSlicerCalculusLogic::acqSliceData(vtkImageReslice* reslice)
{
	//Set background box
	//Set pixels outside a box which is larger than the tightbox to be background
	MyBasic::Range3D bkgBox;
	MyBasic::Range3D imgBox;
	vtkImageData* orgimage = reslice->GetImageDataInput(0);
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
	//图像基准点
	double origin[3];
	orgimage->GetOrigin(origin);

	qDebug() << "image origin:" << origin[0] << " " << origin[1] << "" << origin[2];
	//像素间距
	double spaceing[3];
	orgimage->GetSpacing(spaceing);
	qDebug() << "pixel space:" << spaceing[0] << " " << spaceing[1] << "" << spaceing[2];

	//
	////loadImage into gData
	//gData.loadImage(orgimage, imgBox);

	//Data3D<bool> mat_mask(gData.image.getSize(), true);

	//gData.seeds.set(gData.shifttightBox, UNKNOWN);


	int size = 0;



	int row = dims[1];
	int column = dims[0];
	int li = dims[2];

	uint16* pixel = new uint16[row*column *li]();
	uint16* q = pixel;
	int t = 0;
	QList<double> data;
	for (int k = 0; k < li; k++)
	{
		/*QString fileName = QString("before-%1").arg(k);
		QByteArray array = fileName.toLocal8Bit();
		FILE* file = fopen(array.data(), "wb");
		if (!file)
		{
			qDebug() << fileName << "数据文件打开失败！\n";
		}*/
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				uint16* p = (uint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column +k*row*column] = *p;
				//q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *(p+1);
				//q[i + j*m_gData.wholeRange.col + k*m_gData.wholeRange.sli] = *(p+2);
				if (*p >0)
				{

					std::cout << " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << "  t=" << t << std::endl;
					t++;
					data.append(*p);
				}



			}

		}
		/*fwrite(pixel, sizeof(uint16), column *row, file);
		fclose(file);*/
	}


	double* sliceDataDouble = new double[data.size()]();
	for (int i = 0; i < data.size(); i++)
	{
		sliceDataDouble[i] = data.at(i);
	}

	QHash<QString, double> circleParamsHash;
	circleParamsHash.insert("max", max(sliceDataDouble, data.size()));
	circleParamsHash.insert("min", min(sliceDataDouble, data.size()));
	circleParamsHash.insert("average", aver(sliceDataDouble, data.size()));


	delete[] sliceDataDouble;
	delete[] pixel;

	return circleParamsHash;
}


//--------------朱珊珊添加程序----------------------------------
double vtkSlicerCalculusLogic::max(double a[], int n)
{
	double  max;
	max = a[0];
	int i;
	for (i = 1; i<n; i++)
	{
		if (a[i]>max) max = a[i];
	}
	cout << max << endl;
	return max;
}
double vtkSlicerCalculusLogic::min(double a[], int n)
{
	double min;
	min = a[0];
	int i;
	for (i = 1; i < n; i++)
	{
		if (a[i] < min) min = a[i];
	}
	cout << min << endl;
	return  min;
}

double vtkSlicerCalculusLogic::aver(double a[], int n)
{
	double r = 0;
	int i;
	for (i = 0; i < n; i++)
		r += a[i];
	r /= (double)n;
	cout << r << endl;
	return r;
}
//--------------------
QHash<QString, double> vtkSlicerCalculusLogic::aqc(vtkMRMLVolumeNode* input, vtkMRMLMarkupsFiducialNode* markups)//vtkMRMLScalarVolumeNode,返回值是指针的函数
{
	/*vtkNew<vtkImageData> outputImageData;
	outputImageData->DeepCopy(input->GetImageData());*/
	/*int originalImageExtents[6];
	outputImageData->GetExtent(originalImageExtents);*/
	//基准点数量检测
	//vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4矩阵
	//input->GetRASToIJKMatrix(RASToIJKMatrix);//坐标系转换,得到矩阵坐标
	//int markupNum = markups->GetNumberOfFiducials();
	//qDebug() << markupNum << endl;
	//vector<float> position;  //temp variance to store points' IJK 3 coordinates存储三维坐标

	//vector<float> worldposition;  //temp variance to store points' World 3 coordinates

	//vector<vector <float> > points;  //vector to store sorted markup points in IJK coordinate

	//vector<vector <float> > worldpoints;
	//if (markupNum != 4)//
	//	qDebug() << "error No markupNum =4";
	//else
	//{
	//	for (int i = 0; i < markupNum; i++)
	//	{
	//		double pos[4];//四个点
	//		markups->GetNthFiducialWorldCoordinates(i, pos);//得到每个点的坐标
	//		cout << "WorldPOS:" << pos[0] << ";" << pos[1] << ";" << pos[2] << ";" << pos[3] << endl;//四个点的坐标
	//		float temp[4];
	//		std::copy(pos, pos + 4, temp);
	//		float* ijkpos = RASToIJKMatrix->MultiplyPoint(temp);
	//		cout << "IJKPOS:" << ijkpos[0] << ";" << ijkpos[1] << ";" << ijkpos[2] << ";" << ijkpos[3] << endl;//IJK坐标系下的位置坐标
	//		for (int j = 0; j < 3; j++)
	//		{
	//			position.push_back(ijkpos[j]);
	//			worldposition.push_back(temp[j]);
	//		}
	//		if (i == 0)
	//		{
	//			points.push_back(position);
	//			worldpoints.push_back(worldposition);
	//		}
	//		else
	//		{
	//			int j;
	//			for (j = 0; j<points.size(); j++)
	//			{
	//				if (points.at(j).at(2)>position.at(2))
	//					break;
	//			}
	//			if (j == points.size())
	//			{
	//				points.push_back(position);
	//				worldpoints.push_back(worldposition);
	//			}
	//			else
	//			{
	//				points.insert(points.begin() + j, position);
	//				worldpoints.insert(worldpoints.begin() + j, worldposition);
	//			}
	//		}
	//		position.clear();
	//		worldposition.clear();
	//	}
	//}

	//vector<float> star_first = points.front();
	//vector<float> star_last = points.back();
	//vector<float> box1 = points.at(1);
	//vector<float> box2 = points.at(2);

	//cout << "POINTS[0]:" << star_first[0] << ";" << star_first[1] << ";" << star_first[2] << endl;
	//cout << "POINTS[1]:" << box1[0] << ";" << box1[1] << ";" << box1[2] << endl;
	//cout << "POINTS[2]:" << box2[0] << ";" << box2[1] << ";" << box2[2] << endl;
	//cout << "POINTS[3]:" << star_last[0] << ";" << star_last[1] << ";" << star_last[2] << endl;//points是四个标记点坐标，是矢量

	vtkImageData* outputImageData = input->GetImageData();//得到输入体数据的Imagedata
	int* dims = outputImageData->GetDimensions();//原始图像维数
	double origin[3];
	outputImageData->GetOrigin(origin);
	qDebug() << "o" << origin[0] << " " << origin[1] << " " << origin[2];

	//int dims[3]s;
	//outputImageData->GetDimensions(dims);
	//ExcelExportHelper excel;
	//	QString b;
	//pixel = (double*)malloc(sizeof(double));//
	int n = dims[0] * dims[1];
	double  *pixel;
	pixel = new double[n]();

	//double pixel[990] = { 0 };
	unsigned short *temp=0;
	double temp_value;



	FILE* outFile = fopen("after.txt", "wb");
	if (!outFile)
	{
		qDebug("outFile数据文件打开失败！\n");
	}
	int size = 0;


	int z = dims[2] - 1;
	//for (int z = 1; z < dims[2]; z++)
	//{
	for (int y = 0; y < dims[1]; y++)
	{
		for (int x = 0; x < dims[0]; x++)
		{
			/*if (x < 33 && y < 30 && 2 < z < 4)*/
			/*	if (x < 33 && y < 30)
			{*/
			temp = (unsigned short *)(outputImageData->GetScalarPointer(x, y, z));

			temp_value = temp[0];
			if (temp_value < 60000)
			{

				pixel[x + dims[0] * y] = temp[0];//有个数据类型转换，unsigned char到double
			}
			else
				pixel[x + dims[0] * y] = 0;
			std::cout << pixel[x + dims[0] * y] << " ";

			//}
		}
		std::cout << std::endl;

	}
	/*	std::cout << std::endl;
	}*/

	//未分割原始数据平均最大最小
	double a = max(pixel, n);
	double b = min(pixel, n);
	//aver(pixel, n);
	//--------------------------原始数据归一化,压缩到255级
	/*int n = dims[0] * dims[1];*/
	double  *graypixel;
	graypixel = new double[n]();
	for (int i = 0; i < dims[1]; i++)//行30
	{
		for (int j = 0; j < dims[0]; j++)//列33
		{
			graypixel[j + dims[0] * i] = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;
		}
	}
	//---------------------------------------

	//----------阈值分割，这个分割是对的----------------------------

	int nMaxIter = 100;
	int iDiffRec = 0;
	int F[256] = { 0 }; //直方图数组  

	int iTotalGray = 0;//灰度值和  
	int iTotalPixel = 0;//像素数和  

	int bt;//某点的像素值  

	double iThrehold, iNewThrehold;//阀值、新阀值  
	for (int j = 0; j < n; j++)
	{
		bt = (int)graypixel[j];
		F[bt]++;
	}//统计每个灰度级的个数

	double iMaxGrayValue;
	iMaxGrayValue = max(graypixel, n);
	double iMinGrayValue;
	iMinGrayValue = min(graypixel, n);

	//最大最小灰度值我已经算出，max min
	//20 uchar iMaxGrayValue = 0, iMinGrayValue = 255;//原图像中的最大灰度值和最小灰度值  

	double iMeanGrayValue1, iMeanGrayValue2;//uchar与double区别

	iThrehold = 0;//最终给的阈值
	iNewThrehold = (iMinGrayValue + iMaxGrayValue) / 2;//初始阀值  
	iDiffRec = iMaxGrayValue - iMinGrayValue;

	for (int a = 0; (abs(iThrehold - iNewThrehold) > 0.5) && a < nMaxIter; a++)//迭代中止条件  
	{
		iThrehold = iNewThrehold;
		//小于当前阀值部分的平均灰度值  
		for (int i = iMinGrayValue; i < iThrehold; i++)
		{
			iTotalGray += F[i] * i;//F[]存储图像信息  
			iTotalPixel += F[i];
		}
		iMeanGrayValue1 = (double)(iTotalGray / iTotalPixel);
		//大于当前阀值部分的平均灰度值   
		iTotalPixel = 0;
		iTotalGray = 0;
		for (int j = iThrehold + 1; j < iMaxGrayValue; j++)
		{
			iTotalGray += F[j] * j;//F[]存储图像信息  
			iTotalPixel += F[j];
		}
		iMeanGrayValue2 = (double)(iTotalGray / iTotalPixel);

		iNewThrehold = (iMeanGrayValue2 + iMeanGrayValue1) / 2; //新阀值  
		iDiffRec = abs(iMeanGrayValue2 - iMeanGrayValue1);
	}
	iThrehold = (iThrehold / 255)*(a - b) + b;
	cout << "The Threshold of this Image in imgIteration is:" << iThrehold << endl;
	//-------------------------

	for (int i = 0; i < dims[1]; i++)//行30
	{
		for (int j = 0; j < dims[0]; j++)//列33
		{
			if (pixel[j + dims[0] * i] < iThrehold)//如果小于阈值，输出为0
			{
				double value1 = 0;
				fwrite(&value1, sizeof(double), 1, outFile);
				pixel[j + dims[0] * i] = 0;
			}
			else
			{
				double value = 0;
				value = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;
				fwrite(&value, sizeof(double), 1, outFile);
				//pixel[j + dims[0] * i] = value;

			}
		}

	}
	fclose(outFile);
	QHash<QString, double> circleParamsHash;
	circleParamsHash.insert("max", max(pixel, n));
	circleParamsHash.insert("min", min(pixel, n));
	circleParamsHash.insert("average", aver(pixel, n));

	qDebug() << circleParamsHash.value("max") << endl;



	delete[] pixel;
	delete[] graypixel;
	return circleParamsHash;
}
//---------------添加结束----------------------------------
