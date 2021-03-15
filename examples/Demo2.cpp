#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <opencv2/opencv.hpp>
#include "../src/algorithms/algorithms.h"
#include <sys/stat.h>
#include <chrono>

#if CV_MAJOR_VERSION >= 4
#define CV_LOAD_IMAGE_COLOR cv::IMREAD_COLOR
#endif

int main(int argc, char **argv)
{
 
  std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

  std::string baseDir = "./dataset/frames";
  std::string outputDir = "../dataset/results";
  if (argc > 1)
    baseDir = argv[1];
  std::cout << "Openning: " << baseDir << std::endl;

  /* Background Subtraction Methods */
  auto algorithmsName = BGS_Factory::Instance()->GetRegisteredAlgorithmsName();
  
  std::cout << "List of available algorithms:" << std::endl;
  std::copy(algorithmsName.begin(), algorithmsName.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
  
  for (const std::string& algorithmName : algorithmsName)
  {	
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Running " << algorithmName << std::endl;
    auto bgs = BGS_Factory::Instance()->Create(algorithmName);
    // Create directory to save results
    std::string createDir = "../dataset/results/" + algorithmName + "/";
    const char *charOut = createDir.c_str();
    mkdir(charOut,0777);

	
    auto frame_counter = 0;
    std::cout << "Press 's' to stop:" << std::endl;
    auto key = 0;
    while (key != 's')
    {
      // Capture frame-by-frame
      frame_counter++;
      std::stringstream ss;
      ss << frame_counter;
      auto fileName = baseDir + "/O_SN_01-" + ss.str() + ".bmp";
      auto fileNameOut = outputDir + "/" + algorithmName + "/" + ss.str() + ".png";
      //std::cout << "reading " << fileName << std::endl;
      
      auto img_input = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);

      if (img_input.empty())
        break;

      //cv::imshow("input", img_input);

      cv::Mat img_mask;
      cv::Mat img_bkgmodel;
      try
      {
        bgs->process(img_input, img_mask, img_bkgmodel); // by default, it shows automatically the foreground mask image
	
	//std:: cout << "Output File:" << fileNameOut << std::endl;
        if(!img_mask.empty())
        	cv::imwrite(fileNameOut, img_mask);
        //  do something
      }
      catch (std::exception& e)
      {
        std::cout << "Exception occurred" << std::endl;
        std::cout << e.what() << std::endl;
      }

      key = cv::waitKey(33);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Method: " << algorithmName << " Elapsed time: " << elapsed.count() << " s\n";
    std::cout << "Press 'q' to exit, or anything else to move to the next algorithm:" << std::endl;
    key = cv::waitKey(0); 
    if (key == 'q')
      break;

    cv::destroyAllWindows();
  }

  return 0;
}
