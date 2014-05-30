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

#define SAVE_MID_IMG true
#define SHOW_MID_IMG false
#define DEBUG false



// cutSquare: returns a square used to strip the  boundery of image, according to cut rate.
// The sequence is stored in the specified memory storage.
void cutSquares(const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares, double cutRate)
{  
    std::vector<cv::Point> rect;

    double offset_x = cutRate * image.cols;
    double offset_y = cutRate * image.rows;
    double left =  offset_x;
    double right = image.cols - offset_x;
    double upper = offset_y;
    double lower = image.rows - offset_y;
    rect.push_back(cv::Point(  left, upper));
    rect.push_back(cv::Point(  right, upper));
    rect.push_back(cv::Point(  right, lower));
    rect.push_back(cv::Point(  left, lower));
    squares.push_back(rect);  
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


    if(argc < 2){
        std::cout << "!CutEdge 去边工具：去除图像input_img_path的 cutRate\% 多余边，结果保存到指定目录save_img_path（请确保有目录存在）" << std::endl;
        std::cout << "!USAGE: ./main <input_img_path>  [save_img_path = output/result.tiff] [cutRate = 0.1]" << std::endl;
        return -1;
    }

    cv::string input_img_path = argv[1]; 
    double cutRate = 0.1;
    cv::string save_path = "output/result.tiff";
    

    if(argc>2){
        save_path = argv[2];
    }

    if(argc>3){
        double argv2 = atof(argv[3]);
        if (argv2 >0 && argv2 < 1){
            cutRate = argv2;
        }
    }
 



    cv::Mat input = cv::imread(input_img_path);
    if (input.empty())
    {
        std::cout << "!!! failed imread(), check input img dir:" << input_img_path << std::endl;
        return -1;
    }   

    if (DEBUG) std::cout << "* begin to cut image:" << input_img_path << " @w"<< input.cols <<"h" << input.rows <<" @cutRate:" << cutRate << std::endl;


    std::vector<std::vector<cv::Point> > squares;
    cutSquares(input, squares, cutRate);

    // Draw all rectangular shapes found
    cv::Mat output = input.clone();
    drawSquares(output, squares);
    if (SAVE_MID_IMG) cv::imwrite("mid_process/1_findsquare.png", output);

    // Crop the rectangular shape
    if (squares.size() == 1)
    {    
        cv::Rect box = cv::boundingRect(cv::Mat(squares[0]));
        if (DEBUG) std::cout << "* The location of the box is x:" << box.x << " y:" << box.y << " " << box.width << "x" << box.height << std::endl;

        // Crop the original image to the defined ROI
        cv::Mat crop = input(box);

        /* Anther simple way to get ROI: */
        // double offset_x = cutRate * input.cols;
        // double offset_y = cutRate * input.rows;
        // cv::Mat crop = input(cv::Range(offset_x, input.rows - offset_x),cv::Range(offset_y, input.cols - offset_y));

        if (SHOW_MID_IMG) cv::imshow("crop", crop);
        if (SAVE_MID_IMG) cv::imwrite("mid_process/7_cropped.tiff", crop);

        cv::imwrite(save_path, crop);
        if (DEBUG) std::cout<< "* save result:" << save_path << std::endl;
    }
    else
    {
        std::cout << "* ERROR , no or more than one square(s) found:" << squares.size() << std::endl;

    }

    return 0;
}
