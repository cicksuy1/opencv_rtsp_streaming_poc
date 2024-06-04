
/**
 * @file main.cpp
 * @brief Example program that uses the RTSPStreamer class to stream video.
 */

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <thread>
#include "rtsp-stream.h"

int main() {
    // Set up camera and window for display.
    cv::VideoCapture cap(0);
    cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }

    // Read a frame from the camera.
    cv::Mat frame;
    cap >> frame;

    // Initialize the streamer with the captured frame.
    RTSPStreamer streamer(&frame,true);


    // Start the streaming process.
    streamer.initializeStream(0,0,0);
    streamer.start_stream();
    int counter = 0 ;
    // Main loop for capturing and displaying frames.
    while (true) {
        cap >> frame;
        if (counter==200)
        {
            streamer.stop_streaming();
            std::cout << " Exit succsesfully!"; 
        }
        if (counter == 250)
        {
            streamer.initializeStream(0,0,0);
            streamer.start_stream();
        }
        // Error handling if no frame is captured.
        if (frame.empty()) {
            std::cerr << "Error: No frame captured." << std::endl;
            break;
        }
        std::cout << " Counter: " << counter << std::endl ; 
        counter++;
        // Show the frame in the display window.
        cv::imshow("Display", frame);

        // Break the loop if a key is pressed.
        if (cv::waitKey(10) >= 0) break;
    }

    // Release the camera resource.
    cap.release();

    return 0;
}
