#include "StarShape.h"
#include <algorithm>
#include "common.h"
/*
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Data2Mat.h"

//cv::Mat star_img(100,100,CV_8UC3);
cv::Vec3b color;

Data3D<uchar> img_r;
Data3D<uchar> img_g;
Data3D<uchar> img_b;
*/
// discretize lines in an image,  x2-x1>abs(y2-y1)
void bresenham(int x1,int y1, int x2, int y2, vector<int> &py)
{
	 if((x2-x1)<abs(y2-y1)) { printf("x2-x1 should be larger than abs(y2-y1)\n"); exit(0); }
	 int numPix=x2-x1+1;
	 if(y2==y1)
	 {
		 for(int i=0;i<numPix;i++)
			 py[i]=y1;
		 return;
	 }

     int deltax=x2-x1;
     int deltay =abs(y2-y1);
     int error=deltax/2;
     int ystep;
     int y=y1;
	 if(y1<y2) ystep=1;
	 else ystep=-1;
	 for(int i=0;i<numPix;i++)
	 {
		 py[i]=y;
         error=error-deltay;
		 if(error<0)
		 {
			 y=y+ystep;
		     error=error+deltax;
	     }
	 }
}

void addNextConstraintPixels2D(SparseNLink& slc,int dims[],int seed[], int point[],
					         vector<unsigned char> &markings)

{
	int width=dims[1];
	int height=dims[0];
	bool reverseOrder = false;

	int index[2]; int max=0;
	for(int i=0;i<2;i++)
	{
		if(abs(seed[i]-point[i])>max) { max=abs(seed[i]-point[i]); index[0]=i; }
	}
	int j=1;
	for(int i=0;i<2;i++)
	{
		if(i!=index[0]) { index[j]=i;}
	}

	int x1,x2,y1,y2;
	if(point[index[0]]<seed[index[0]])
	{
		reverseOrder=true;
		x1=point[index[0]];
		x2=seed[index[0]];
		y1=point[index[1]];
		y2=seed[index[1]];
	}
	else {
		x1=seed[index[0]];
		x2=point[index[0]];
		y1=seed[index[1]];
		y2=point[index[1]];
	}

	int numPix=x2-x1+1;
	vector<vector<int> > pt(2,vector<int>(numPix,0));
	vector<int> pixels(numPix);  //pixel ID for current dimension

	for(int i=0;i<numPix;i++)
	{
		pt[index[0]][i]=x1+i;
	}
	bresenham(x1,y1,x2,y2,pt[index[1]]);

	/*
	for(int i=0;i<numPix;i++)
	{
	   star_img.at<cv::Vec3b>(pt[0][i],pt[1][i]) = color;
	}
*/
	for(int i=0;i<numPix;i++)
	{
		pixels[i]=pt[0][i]*width+pt[1][i];
	}

	if ( reverseOrder ){
		int i;
		for (  i = 1; i < numPix; i++ ){
		//	if ( markings[pixels[i-1]] == 0 ){
				markings[pixels[i-1]] = 1;
				int id1=pixels[i-1];
				int id2=pixels[i];
			//	g->add_edge(id1,id2,STAR_HUGE,0);
				slc.id1.push_back(id1);
				slc.id2.push_back(id2);
				slc.cap.push_back(STAR_HUGE);
				slc.rev_cap.push_back(0);
		}

	}
	else {
		int i;
		for (  i  = numPix-1; i > 0; i-- ){
	//		if ( markings[pixels[i]] == 0 ){
				markings[pixels[i]] = 1;
				int id1=pixels[i];
				int id2=pixels[i-1];
			//	g->add_edge(id1,id2,STAR_HUGE,0);
				slc.id1.push_back(id1);
				slc.id2.push_back(id2);
				slc.cap.push_back(STAR_HUGE);
				slc.rev_cap.push_back(0);

		}

	}

}


//---------------------------------------------------------
// chooses all image pixels (not just the border) in random order
SparseNLink starshape2D(const MyBasic::Size2D& dim, const MyBasic::Index2D& seed)
{ //vector pixels is the pixelID for pixels on this slice
    SparseNLink result;

   // star_img.resize(dim.nRow,dim.nCol);
  //  cv::RNG rng(12345);
    //star_img = cv::Mat(dim.nRow,dim.nCol,CV_8UC3);

    int dims[2] = {dim.nRow,dim.nCol};
    int seeds[2] = {seed.row,seed.col};

	int num_pixels=dim.nRow*dim.nCol;
	vector<unsigned char> markings(num_pixels,0);
	vector<int> pixelOrder(num_pixels);

	unsigned int numPixels = 0;
	int seedX=seed.col, seedY=seed.row;
	int width=dim.nCol, height=dim.nRow;
	markings[seedX+seedY*width] = 1;

	for ( int i = 0; i < num_pixels; i++ )
	{
		pixelOrder[i] = i;
    }

    random_shuffle(pixelOrder.begin(),pixelOrder.end());

	int unmarked=0;
	for ( int i = 0; i < num_pixels; i++ )
	{
		if ( markings[pixelOrder[i]] == 0 )
		{

			int nextY = pixelOrder[i]/width;
			int nextX = pixelOrder[i]-nextY*width;
			int point[2]={nextY,nextX};

			//used for debug
		//	color = cv::Vec3b(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
			addNextConstraintPixels2D(result,dims,seeds,point,markings);
	//		if(markings[pixelOrder[i]]==0) printf("%d %d %d\n",i,nextY,nextX);
			if(markings[pixelOrder[i]]==0){ unmarked++; }
		}
     }

	if(unmarked>0) printf("2D starshape unmarked pixels: %d\n",unmarked);

	return result;

}


/////////////////////////////////////////////////////////////////////////////////////
//             3D starshape
////////////////////////////////////////////////////////////////////////////////////
void addNextConstraintPixels3D(SparseNLink& slc, int dims[3],int seed[3], int point[3],
					         vector<unsigned char> &markings)

{
	bool reverseOrder = false;
	int index[3]; int max=0;
	for(int i=0;i<3;i++)
	{
		if(abs(seed[i]-point[i])>max) { max=abs(seed[i]-point[i]); index[0]=i; }
	}
	int j=1;
	for(int i=0;i<3;i++)
	{
		if(i!=index[0]) { index[j]=i; j++;}
	}

	int x1,y1,x2,y2;
	// ensure that x2>=x1
	x1=seed[index[0]]; x2=point[index[0]];
	if(point[index[0]]<seed[index[0]]) {
		reverseOrder=true;
		x1=point[index[0]];
		x2=seed[index[0]];
	}

	int numPix=x2-x1+1;
	vector<vector<int> > pt(3,vector<int>(numPix,0));
	//initial values for direction index[0]
	for(int i=0;i<numPix;i++)
		pt[index[0]][i]=x1+i;

	// get coordinates for other directions
	for(int i=1;i<3;i++)
	{

		if(reverseOrder) {
			y1=point[index[i]];
			y2=seed[index[i]];
		}
		else{
			y1=seed[index[i]];
			y2=point[index[i]];
		}

		bresenham(x1,y1,x2,y2,pt[index[i]]);
	}

	// get current pixel ID
	vector<int> pixels(numPix);
	for(int i=0;i<numPix;i++)
	{
		pixels[i]=pt[0][i]*dims[1]*dims[2]+pt[1][i]*dims[2]+pt[2][i];
	}
/*
    for(int i=0;i<numPix;i++)
    {
        img_r.at(pt[0][i],pt[1][i],pt[2][i]) = color[0];
        img_g.at(pt[0][i],pt[1][i],pt[2][i]) = color[1];
        img_b.at(pt[0][i],pt[1][i],pt[2][i]) = color[2];
    }
*/
	if ( reverseOrder ){
		int i;
		for (  i = 0; i < numPix-1; i++ ){
	//		if ( markings[pixels[i]] == 0 ){
				markings[pixels[i]] = 1;
				int id1=pixels[i];
				int id2=pixels[i+1];
			//	g->add_edge(id1,id2,STAR_HUGE,0);
				slc.id1.push_back(id1);
				slc.id2.push_back(id2);
				slc.cap.push_back(STAR_HUGE);
				slc.rev_cap.push_back(0);
		//	}
		//	else break; // break if current pixel has been added to the constraint
		}
	}

	else {
		int i;
		for (  i  = numPix-1; i > 0; i-- ){
		//	if ( markings[pixels[i]] == 0 ){
				markings[pixels[i]] = 1;
				int id1=pixels[i];
				int id2=pixels[i-1];
				//g->add_edge(id1,id2,STAR_HUGE,0);
				slc.id1.push_back(id1);
				slc.id2.push_back(id2);
				slc.cap.push_back(STAR_HUGE);
				slc.rev_cap.push_back(0);
	//		}
		//	else break; // break if current pixel has been added to the constraint
		}

	}

}


//---------------------------------------------------------
// chooses all image pixels (not just the border) in random order
SparseNLink starshape3D(const MyBasic::Size3D& dim, const MyBasic::Index3D& _seed)
{
/*
    img_r.resize(dim);
    img_g.resize(dim);
    img_b.resize(dim);

    cv::RNG rng(12345);
*/
    SparseNLink result;

    int dims[3] = {dim.nSli,dim.nRow,dim.nCol};
    int seed[3] = {_seed.sli,_seed.row,_seed.col};

	int num_pixels=dims[0]*dims[1]*dims[2];
	vector<unsigned char> markings(num_pixels,0);
	vector<int> pixelOrder(num_pixels);
	int numPixels = 0;

	int seed_id=seed[0]*dims[1]*dims[2]+seed[1]*dims[2]+seed[2];
	markings[seed_id] = 1;

	for ( int i = 0; i < num_pixels; i++ )
		pixelOrder[i] = i;

	random_shuffle(pixelOrder.begin(),pixelOrder.end());

	int num=0;
	int diff=0;
	int next[3]; int unmarked=0;
	for ( int i = 0; i < num_pixels; i++ )
		if ( markings[pixelOrder[i]] == 0 )
		{

			int nextS = pixelOrder[i]/(dims[1]*dims[2]);
			int nextR = (pixelOrder[i]-nextS*dims[1]*dims[2])/dims[2];
			int nextC=pixelOrder[i]-nextS*dims[1]*dims[2]-nextR*dims[2];

			next[0]=nextS; next[1]=nextR; next[2]=nextC;
		//    color = cv::Vec3b(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
			addNextConstraintPixels3D(result,dims,seed,next,markings);
			diff = result.id1.size()-num;
			num = result.id1.size();
			if(markings[pixelOrder[i]]==0) unmarked++; //printf("%d %d %d %d\n",i,nextR,nextC,nextS);
		}
	if(unmarked>0) printf("3D starshape unmarked pixels: %d\n",unmarked);
/*
	cv::Mat mat_r = TypeCast::toGrayscale(img_r,false);
	cv::Mat mat_g = TypeCast::toGrayscale(img_g,false);
	cv::Mat mat_b = TypeCast::toGrayscale(img_b,false);

    cv::Mat mat_img(mat_r.rows,mat_r.cols,CV_8UC3);
    for(int r=0;r<mat_r.rows;r++)
    {
       for(int c=0;c<mat_r.cols;c++)
       {
           mat_img.at<cv::Vec3b>(r,c)[0] = mat_r.at<uchar>(r,c);
           mat_img.at<cv::Vec3b>(r,c)[1] = mat_g.at<uchar>(r,c);
           mat_img.at<cv::Vec3b>(r,c)[2] = mat_b.at<uchar>(r,c);
       }
    }

    cv::namedWindow( "star 3d", cv::WINDOW_AUTOSIZE );
    cv::imshow("star 3d",mat_img);
    cv::waitKey(0);
*/
	return result;

}

