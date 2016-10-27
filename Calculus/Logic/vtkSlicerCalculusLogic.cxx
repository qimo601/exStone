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


double vtkSlicerCalculusLogic::s_sliceThick;//任意角度切片厚度
double vtkSlicerCalculusLogic::s_uWater;//水的衰减系数
double vtkSlicerCalculusLogic::s_materialThick;//材料厚度
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



QHash<QString, double> vtkSlicerCalculusLogic::acqSliceData(vtkImageReslice* reslice, vtkMRMLSliceNode* sliceNode, vtkMRMLVolumeNode* volumeNode)
{



	vtkSmartPointer<vtkMatrix4x4> sliceToRAS = sliceNode->GetSliceToRAS();//slice原点和法向量矩阵
	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4矩阵
	vtkSmartPointer<vtkMatrix4x4> IJKToRASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//转换矩阵
	volumeNode->GetRASToIJKMatrix(RASToIJKMatrix);
	volumeNode->GetIJKToRASMatrix(IJKToRASMatrix);

	double normal[4];//法向量
	normal[0] = sliceToRAS->GetElement(0, 2);
	normal[1] = sliceToRAS->GetElement(1, 2);
	normal[2] = sliceToRAS->GetElement(2, 2);
	normal[3] = 1;

	qDebug() << "normal:" << normal[0] << " " << normal[1] << " " << normal[2] << " " << normal[3];


	double origin[4];//法向量
	origin[0] = sliceToRAS->GetElement(0, 3);
	origin[1] = sliceToRAS->GetElement(1, 3);
	origin[2] = sliceToRAS->GetElement(2, 3);
	origin[3] = 1;

	qDebug() << "origin:" << origin[0] << " " << origin[1] << " " << origin[2] << " " << origin[3];

	//计算平面方程

	double A = normal[0];
	double B = normal[1];
	double C = normal[2];
	double d = -(A * origin[0] + B * origin[1] + C * origin[2]);//平面方程D


	//得到所有>0点IJK坐标
	vtkImageData* orgimage = reslice->GetImageDataInput(0);

	//尺寸长宽高
	int* dims = orgimage->GetDimensions();
	QList<double*> pointList;//结石的有效点


	int row = dims[1];
	int column = dims[0];
	int li = dims[2];

	uint16* pixel = new uint16[row*column *li]();
	uint16* q = pixel;
	int t = 0;
	QString fileName = "before";
	QByteArray array = fileName.toLocal8Bit();
	FILE* file = fopen(array.data(), "wb");
	if (!file)
	{
		qDebug() << fileName << "数据文件打开失败！\n";
	}
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
				q[i + j*column + k*row*column] = *p;
				if (*p >0)
				{

					//std::cout << " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << "  t=" << t << std::endl;
					t++;
					double*point = new double[5];
					point[0] = j;
					point[1] = i;
					point[2] = k;
					point[3] = 1;
					point[4] = *p;

					pointList.append(point);
				}



			}

		}

	}

	fwrite(pixel, sizeof(uint16), column *row*li, file);
	fclose(file);

	//将所有点转换成RAS坐标

	QList<double*> rasPointList;
	//筛选一下容器点到面的距离<2
	QList<double*> resultPointList;
	
	for (int m1 = 0; m1 < pointList.count(); m1++)
	{
		double*point = pointList.at(m1);
		double* rasPoint = IJKToRASMatrix->MultiplyDoublePoint(point);
		double distance = qAbs(A*rasPoint[0] + B*rasPoint[1] + C*rasPoint[2] + d) / qSqrt(A*A + B*B + C*C);
		if (distance < vtkSlicerCalculusLogic::s_sliceThick)//distance有变化，但是坐标值输出看不出变化
		{
			//qDebug() << "m1:" << m1 << " ijkPoint :" << point[0] << " " << point[1] << " " << point[2] << " " << point[3] <<  ""<< point[4];
			//qDebug() << "m1:" << m1 << " rasPoint :" << rasPoint[0] << " " << rasPoint[1] << " " << rasPoint[2] << " " << rasPoint[3];//IJK转换到RAS，坐标有变化
			double* resultPoint = RASToIJKMatrix->MultiplyDoublePoint(rasPoint);
			//qDebug() << "distance" << distance << " resultPointijk:" << resultPoint[0] << " " << resultPoint[1] << " " << resultPoint[2] << " " << resultPoint[3] <<" " << resultPoint[4];
			double* result = new double[5]();
			result[0] = resultPoint[0];
			result[1] = resultPoint[1];
			result[2] = resultPoint[2];
			result[3] = resultPoint[3];
			result[4] = point[4];
			resultPointList.append(result);
			qDebug() << "distance" << distance << " resultPointijk:" << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " " << result[4];

		}
		rasPointList.append(rasPoint);
	}
	qDebug() << "rasPointList size" << rasPointList.size() << " resultPointList size:" << resultPointList.size()<<endl;


	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (resultPointList.size() > 0)
	{
		sliceDataDouble = new double[resultPointList.size()]();
		for (int i = 0; i < resultPointList.size(); i++)
		{
			double* point = resultPointList.at(i);
			sliceDataDouble[i] = point[4];//得到CT值
		}
		circleParamsHash.insert("max", max(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, resultPointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, resultPointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, resultPointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	
		if (sliceDataDouble!=0)
			delete[] sliceDataDouble;
	}

	
	
	if (pixel!=0)
		delete[] pixel;
	//释放所有点
	for (int index = 0; index < pointList.size(); index++)
	{
		double* p = pointList.at(index);
		delete[] p;
	}
	//释放所有点
	for (int index = 0; index < resultPointList.size(); index++)
	{
		double* p = resultPointList.at(index);
		delete[] p;
	}

	std::cout << "sliceToRAS:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " [" << i << "][" << j << "] " << sliceToRAS->GetElement(i, j);
		}
		std::cout << std::endl;
	}

	//reslice->SetResliceAxes(resliceAxes);
	std::cout << "RAS to IJK:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " [" << i << "][" << j << "] " << RASToIJKMatrix->GetElement(i, j);
		}
		std::cout << std::endl;
	}

	std::cout << "IJK to RAS:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << " [" << i << "][" << j << "] " << IJKToRASMatrix->GetElement(i, j);
		}
		std::cout << std::endl;
	}




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
//--------------------------------------------------------------
//获取圆的像素数据 
QHash<QString, double> vtkSlicerCalculusLogic::aqcCircleData(vtkMRMLVolumeNode* input, vtkMRMLMarkupsFiducialNode* markups)//vtkMRMLScalarVolumeNode,返回值是指针的函数
{
	/*vtkNew<vtkImageData> outputImageData;
	outputImageData->DeepCopy(input->GetImageData());*/
	/*int originalImageExtents[6];
	outputImageData->GetExtent(originalImageExtents);*/
	//基准点数量检测
	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4矩阵
	input->GetRASToIJKMatrix(RASToIJKMatrix);//坐标系转换
	int markupNum = markups->GetNumberOfFiducials();
	qDebug() << markupNum << endl;
	vector<float> position;  //temp variance to store points' IJK 3 coordinates存储三维坐标

	vector<float> worldposition;  //temp variance to store points' World 3 coordinates

	vector<vector <float> > points;  //vector to store sorted markup points in IJK coordinate

	vector<vector <float> > worldpoints;
	if (markupNum != 4)//
		qDebug() << "error No markupNum =4";
	else
	{
		for (int i = 0; i < markupNum; i++)
		{
			double pos[4];//四个点
			markups->GetNthFiducialWorldCoordinates(i, pos);//得到每个点的坐标
			cout << "WorldPOS:" << pos[0] << ";" << pos[1] << ";" << pos[2] << ";" << pos[3] << endl;//四个点的坐标
			float temp[4];
			std::copy(pos, pos + 4, temp);
			float* ijkpos = RASToIJKMatrix->MultiplyPoint(temp);
			cout << "IJKPOS:" << ijkpos[0] << ";" << ijkpos[1] << ";" << ijkpos[2] << ";" << ijkpos[3] << endl;//IJK坐标系下的位置坐标
			for (int j = 0; j < 3; j++)
			{
				position.push_back(ijkpos[j]);
				worldposition.push_back(temp[j]);
			}
			if (i == 0)
			{
				points.push_back(position);
				worldpoints.push_back(worldposition);
			}
			else
			{
				int j;
				for (j = 0; j<points.size(); j++)
				{
					if (points.at(j).at(2)>position.at(2))
						break;
				}
				if (j == points.size())
				{
					points.push_back(position);
					worldpoints.push_back(worldposition);
				}
				else
				{
					points.insert(points.begin() + j, position);
					worldpoints.insert(worldpoints.begin() + j, worldposition);
				}
			}
			position.clear();
			worldposition.clear();
		}
	}

	vector<float> star_first = points.front();
	vector<float> star_last = points.back();
	vector<float> box1 = points.at(1);
	vector<float> box2 = points.at(2);

	cout << "POINTS[0]:" << star_first[0] << ";" << star_first[1] << ";" << star_first[2] << endl;
	cout << "POINTS[1]:" << box1[0] << ";" << box1[1] << ";" << box1[2] << endl;
	cout << "POINTS[2]:" << box2[0] << ";" << box2[1] << ";" << box2[2] << endl;
	cout << "POINTS[3]:" << star_last[0] << ";" << star_last[1] << ";" << star_last[2] << endl;//points是四个标记点坐标，是矢量

	vtkImageData* outputImageData = input->GetImageData();//得到输入体数据的Imagedata
	int* dims = outputImageData->GetDimensions();//原始图像维数
	double origin[3];
	outputImageData->GetOrigin(origin);
	qDebug() << "o" << origin[0] << " " << origin[1] << " " << origin[2];

	double spaceing[3];
	outputImageData->GetSpacing(spaceing);
	qDebug() << "像素间隔:" << spaceing[0] << " " << spaceing[1] << "" << spaceing[2] << endl;

	//int dims[3]s;
	//outputImageData->GetDimensions(dims);
	//ExcelExportHelper excel;
	//	QString b;
	//pixel = (double*)malloc(sizeof(double));//
	int n = dims[0] * dims[1];
	double  *pixel;
	pixel = (double*)malloc(sizeof(double)*n);//指针类型

	//double pixel[990] = { 0 };
	unsigned short *temp = { 0 };
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

				pixel[x + dims[0] * y] = temp_value;//有个数据类型转换，unsigned char到double
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
	graypixel = (double*)malloc(sizeof(double)*n);
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
	int counter = 0;
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
				++counter;
				double value = 0;
				value = ((pixel[j + dims[0] * i] - b) / (a - b)) * 255;//输出的数据是对的
				fwrite(&value, sizeof(double), 1, outFile);
				//pixel[j + dims[0] * i] = value;


			}
		}

	}
	//分割后像素数据
	double  *pixelsegmentation;
	pixelsegmentation = (double*)malloc(sizeof(double)*counter);
	int i = 0;
	for (int j = 0; j < n; j++)
	{
		if (pixel[j])
		{
			pixelsegmentation[i] = pixel[j];
			std::cout << pixelsegmentation[i] << " ";
			i++;
			continue;//结束整个循环，continue结束单次循环
		}
		//*pixelsegmentation++;//pixelsegmentation++与之区别
	}

	fclose(outFile);
	QHash<QString, double> circleParamsHash;
	circleParamsHash.insert("max", max(pixelsegmentation, counter));
	circleParamsHash.insert("min", min(pixelsegmentation, counter));
	circleParamsHash.insert("average", aver(pixelsegmentation, counter));
	circleParamsHash.insert("AOD", AOD(pixelsegmentation, counter, vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	circleParamsHash.insert("IOD", IOD(pixelsegmentation, counter, vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
	qDebug() << circleParamsHash.value("max") << endl;
	return circleParamsHash;


}


//---------------添加结束----------------------------------
double vtkSlicerCalculusLogic::AOD(double a[], int n, double m, double d)//n 像素个数 ,m是水的衰减系数,d材料厚度(每个像素点的厚度)算每点OD
{
	double r = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((a[i] / 1000)*m + m)*d;
		a[i] = log10(exp(r));//每一点指数
	}
	//cout << aver(a, n) << endl;
	return aver(a, n);
}
double vtkSlicerCalculusLogic::IOD(double a[], int n, double m, double d)
{
	double r = 0;
	double sum = 0;
	for (int i = 0; i < n; i++)
	{
		r = ((a[i] / 1000)*m + m)*d;
		a[i] = log10(exp(r));//每一点指数
		sum += a[i];//求和
	}
	//cout << sum << endl;
	return sum;
}

//-----------------------------------------------------------------------
//获取垂直切面的slice数据
QHash<QString, double> vtkSlicerCalculusLogic::acqSliceVerticalData(vtkMRMLVolumeNode* input,double offset,QString direction)
{
	//得到所有>0点IJK坐标
	vtkImageData* orgimage = input->GetImageData();


	
	//尺寸长宽高
	int* dims = orgimage->GetDimensions();
	QList<double*> pointList;//结石的有效点


	int row = dims[1];
	int column = dims[0];
	int li = dims[2];




	vtkSmartPointer<vtkMatrix4x4> RASToIJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();//4*4矩阵
	input->GetRASToIJKMatrix(RASToIJKMatrix);

	double* rasPoint = new double[4]();
	memset(rasPoint,0,4);
	if (direction=="X")
		rasPoint[0] = offset;
	if (direction == "Y")
		rasPoint[1] = offset;
	if (direction == "Z")
		rasPoint[2] = offset;
	rasPoint[3] = 1;

	double* ijkPoint = RASToIJKMatrix->MultiplyDoublePoint(rasPoint);
	int intOffset;
	if (direction == "X")
		intOffset= ijkPoint[0];
	if (direction == "Y")
		intOffset = ijkPoint[1];
	if (direction == "Z")
		intOffset = ijkPoint[2];
	uint16* pixel = new uint16[row*column *li]();
	uint16* q = pixel;
	int index = 0;//有效方向
	int t = 0;
	for (int k = 0; k < li; k++)
	{
		
		for (int j = 0; j < row; j++)
		{
			for (int i = 0; i < column; i++)
			{

				uint16* p = (uint16*)(orgimage->GetScalarPointer(i, j, k));
				q[i + j*column + k*row*column] = *p;
				if (*p >0)
				{

					
					if (direction == "X" || direction == "x")
					{
						index = i;
					}
					else if (direction == "Y" || direction == "y")
					{
						index = j;
					}
					else if (direction == "Z" || direction == "z")
					{
						index = k;
					}

					if (index == intOffset)
					{
						double*point = new double[5];
						point[0] = j;
						point[1] = i;
						point[2] = k;
						point[3] = 1;
						point[4] = *p;

						pointList.append(point);
						t++;
						std::cout << "t=" << t<< " p[" << i << "][" << j << "]" << "[" << k << "]" << *p << std::endl;
					}
				}



			}

		}

	}
	double* sliceDataDouble;
	QHash<QString, double> circleParamsHash;
	if (pointList.size() > 0)
	{
		sliceDataDouble = new double[pointList.size()]();
		for (int i = 0; i < pointList.size(); i++)
		{
			double* point = pointList.at(i);
			sliceDataDouble[i] = point[4];//得到CT值
		}

		circleParamsHash.insert("max", max(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("min", min(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("average", aver(sliceDataDouble, pointList.size()));
		circleParamsHash.insert("AOD", AOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		circleParamsHash.insert("IOD", IOD(sliceDataDouble, pointList.size(), vtkSlicerCalculusLogic::s_uWater, vtkSlicerCalculusLogic::s_materialThick));
		if (sliceDataDouble!=0)
			delete[] sliceDataDouble;

	}

	if (pixel != 0)
		delete[] pixel;
	//释放所有点
	for (int index = 0; index < pointList.size(); index++)
	{
		double* p = pointList.at(index);
		delete[] p;
	}
	

	return circleParamsHash;
}
