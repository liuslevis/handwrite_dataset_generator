/*
    TO RUN:
    cmake .
    make
    ./main args ...
*/

#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define BINARY_THRESHOLD 50 
#define SAVE_MID_IMG true
#define SHOW_MID_IMG false
#define APDATIVE_THRESH_BLOCK_SIZE 3
#define APDATIVE_THRESH_C 5
#define DEBUG false

#define ERODE_ELE_SIZE 5

// angle: helper function.
// Finds a cosine of angle between vectors from pt0->pt1 and from pt0->pt2.
double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

std::vector<cv::Point> squares2point(std::vector<std::vector<cv::Point> > squares){
    std::vector<cv::Point> points;
    for (std::vector<std::vector<cv::Point> >::iterator i = squares.begin(); i != squares.end(); ++i)
    {
        for (std::vector<cv::Point>::iterator j = (*i).begin(); j != (*i).end(); ++j)
        {
            points.push_back( *j );
        }
    }

    return points;
}

// findSquares: returns sequence of squares detected on the image.
// The sequence is stored in the specified memory storage.
void findSquares(const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares)
{  
    cv::Mat pyr, timg;

    // Down-scale and up-scale the image to filter out small noises
    cv::pyrDown(image, pyr, cv::Size(image.cols/2, image.rows/2));
    cv::pyrUp(pyr, timg, image.size());

    // Apply Canny with a threshold of 50
    cv::Canny(timg, timg, 0, 50, 5);

    // Dilate canny output to remove potential holes between edge segments
    cv::dilate(timg, timg, cv::Mat(), cv::Point(-1,-1));

    // find contours and store them all as a list 
    std::vector<std::vector<cv::Point> > contours;           
    cv::findContours(timg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    for( size_t i = 0; i < contours.size(); i++ ) // Test each contour
    {
        // Approximate contour with accuracy proportional to the contour perimeter
        std::vector<cv::Point> approx;   
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

        // Square contours should have 4 vertices after approximation
        // relatively large area (to filter out noisy contours)
        // and be convex.
        // Note: absolute value of an area is used because
        // area may be positive or negative - in accordance with the
        // contour orientation
        if( approx.size() == 4 &&
            fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
            cv::isContourConvex(cv::Mat(approx)) )
        {
            double maxCosine = 0;

            for (int j = 2; j < 5; j++)
            {
                // Find the maximum cosine of the angle between joint edges
                double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            // If cosines of all angles are small
            // (all angles are ~90 degree) then write quandrange
            // vertices to resultant sequence
            if( maxCosine < 0.3 )
                squares.push_back(approx);
        }
    }         
}


// drawSquares: function draws all the green squares found in the image
void drawSquares( cv::Mat& image, const std::vector<std::vector<cv::Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const cv::Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        cv::polylines(image, &p, &n, 1, true, cv::Scalar(0,255,0), 2, CV_AA);
    }

    if (SHOW_MID_IMG) cv::imshow("drawSquares", image);
}

// drawSquares: function draws all the green squares found in the image
void drawPoints( cv::Mat& image, const std::vector<cv::Point> points, const int radius){
    for( size_t i = 0; i < points.size(); i++ )
    {
        cv::Point p =  points[i];
        int thickness = 1;
        int lineType=8;
        int shift=0;
        circle(image, p, radius, cv::Scalar(255,0,0), thickness,lineType, shift);
    }

    if (SHOW_MID_IMG) cv::imshow("drawPoints", image);
}

int main(int argc, char* argv[])
{
    // Load input image (colored, 3-channel)

    std::string result_dir = "output/";
    std::string img_path = "";
    if(argc <= 1){
        std::cout << "!BoxExtraction 提取img_path图像内，方格字块中的图像，结果存储到文件result_dir中" << std::endl;
        std::cout << "!USAGE: ./main <img_path> [result_dir=./output]" << std::endl;
        return -1;
    }
    if(argc > 1){
        img_path = argv[1];
    }

    // Resize
    cv::Mat bigger = cv::imread(argv[1]);
    double scale = 0.8;
    cv::Size dsize = cv::Size(bigger.cols*scale,bigger.rows*scale);
    cv::Mat input = cv::Mat(dsize,CV_32S);
    cv::resize(bigger,input,dsize);

    // origin size
    // cv::Mat input = cv::imread(argv[1]);

    if(argc > 2){
        result_dir = argv[2];
    }

    if (input.empty())
    {
        std::cout << "!!! failed imread(), check dir:" << argv[1] << std::endl;
        return -1;
    }   



    // Convert input image to grayscale (1-channel)
    cv::Mat grayscale = input.clone();
    cv::cvtColor(input, grayscale, cv::COLOR_BGR2GRAY);
    if (SAVE_MID_IMG) cv::imwrite("mid_process/1_gray.png", grayscale);

    // Threshold to binarize the image and get rid of the shoe ( Try adaptiveThreshold or normal both!)
    cv::Mat binary;
    // cv::threshold(grayscale, binary, BINARY_THRESHOLD , 255, cv::THRESH_BINARY_INV); 
    cv::adaptiveThreshold(grayscale, binary, BINARY_THRESHOLD, cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV, APDATIVE_THRESH_BLOCK_SIZE,APDATIVE_THRESH_C );
    if (SHOW_MID_IMG) cv::imshow("Binary image", binary);
    if (SAVE_MID_IMG) cv::imwrite("mid_process/2_binary.png", binary);

    // Find the contours in the thresholded image
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // Fill the areas of the contours with BLUE (hoping to erase everything inside a rectangular shape)
    cv::Mat blue = input.clone();      
    for (size_t i = 0; i < contours.size(); i++)
    {
        std::vector<cv::Point> cnt = contours[i];
        double area = cv::contourArea(cv::Mat(cnt));               

        //std::cout << "* Area: " << area << std::endl; 
        cv::drawContours(blue, contours, i, cv::Scalar(255, 0, 0), 
                         CV_FILLED, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );         
    }       

    if (SHOW_MID_IMG) cv::imshow("Countours Filled", blue);  
    if (SAVE_MID_IMG) cv::imwrite("mid_process/3_contours.png", blue);  


    // Convert the blue colored image to binary (again), and we will have a good rectangular shape to detect
    cv::Mat gray;
    cv::cvtColor(blue, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, BINARY_THRESHOLD, 255, cv::THRESH_BINARY_INV);
    if (SHOW_MID_IMG) cv::imshow("binary2", binary);
    if (SAVE_MID_IMG) cv::imwrite("mid_process/4_binary2.png", binary);

    // Erode & Dilate to isolate segments connected to nearby areas
    int erosion_type = cv::MORPH_RECT; 
    int erosion_size = ERODE_ELE_SIZE;
    cv::Mat element = cv::getStructuringElement(erosion_type, 
                                                cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), 
                                                cv::Point(erosion_size, erosion_size));
    cv::erode(binary, binary, element);
    cv::dilate(binary, binary, element);
    if (SHOW_MID_IMG) cv::imshow("Morphologic Op", binary); 
    if (SAVE_MID_IMG) cv::imwrite("mid_process/5_morpho.png", binary);

    // Ok, let's go ahead and try to detect all rectangular shapes
    std::vector<std::vector<cv::Point> > squares;
    findSquares(binary, squares);
    
    std::cout << "* img:" << img_path << " found rectangle number:" << squares.size() << " Save to:"<<result_dir<< std::endl;

    // Draw all rectangular shapes found
    cv::Mat output = input.clone();
    drawPoints(output,squares2point(squares),6);
    drawSquares(output, squares);
    if (SAVE_MID_IMG) cv::imwrite("mid_process/6_output.png", output);

    // Crop the rectangular shape
    if (squares.size() == 1)
    {    
        cv::Rect box = cv::boundingRect(cv::Mat(squares[0]));
        if(DEBUG) std::cout << "* The location of the box is x:" << box.x << " y:" << box.y << " " << box.width << "x" << box.height << std::endl;

        // Crop the original image to the defined ROI
        cv::Mat crop = input(box);
        if (SHOW_MID_IMG) cv::imshow("crop", crop);
        if (SAVE_MID_IMG) cv::imwrite("mid_process/7_box.tiff", crop);
    }
    else
    {
        for( int i = 0; i < squares.size(); i++){
            cv::Rect box = cv::boundingRect(cv::Mat(squares[i]));
            if(DEBUG) std::cout << "* The location of the box is x:" << box.x << " y:" << box.y << " " << box.width << "x" << box.height << std::endl;

            // Crop the original image to the defined ROI
            cv::Mat crop = input(box);
            if (SHOW_MID_IMG) cv::imshow("crop", crop);

            char savename[50];
            sprintf(savename,"%s/%d_cropped.tiff",result_dir.c_str(),i);
            if (SAVE_MID_IMG) cv::imwrite(savename, crop);

        }

    }

    // Wait until user presses key
    if(SHOW_MID_IMG) cv::waitKey(0);

    return 0;
}
