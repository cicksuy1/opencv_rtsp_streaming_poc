#include "rtsp-stream.h"
#include <sstream>
#include <iostream>

// Constructor: Initializes the streamer with a pointer to a frame.
RTSPStreamer::RTSPStreamer(cv::Mat* framePtr, bool streaming = true) : frame(framePtr), slicedRect(0, 0, 160, 120) {
    fps = 30.0; // Default FPS
    // Set the frame dimensions.
    this->roi_width = ROI_DEFAULT_WIDTH;
    this->roi_height = ROI_DEFAULT_HEIGHT;
    this->bitrate = DEFAULT_BITRATE * (std::pow(10,3));
    this->currRoiX =0;
    this->currRoiY = 0;
    frameWidth = frame->cols;
    frameHeight = frame->rows;
}

// Updated constructor with adjustable parameters.
RTSPStreamer::RTSPStreamer(cv::Mat* framePtr ,bool streaming , double fps = 30.0, int bitrate = 400000, int width = ROI_DEFAULT_WIDTH, int height = ROI_DEFAULT_HEIGHT) : 
    frame(framePtr), slicedRect(0, 0, width, height), fps(fps) {
    //set inside params
    this->bitrate = bitrate;
    this->currRoiX =0;
    this->currRoiY = 0;
    this->fps = fps;
    this->roi_width = width;
    this->roi_height = height;
    // Set frame dimensions.
    frameWidth = frame->cols;
    frameHeight = frame->rows;
}



void RTSPStreamer::set_roi(int x, int y) {
    // Ensure the rectangle stays within the frame.
    slicedRect.x = std::max(0, std::min(x - slicedRect.width / 2, frameWidth - slicedRect.width));
    slicedRect.y = std::max(0, std::min(y - slicedRect.height / 2, frameHeight - slicedRect.height));
    currRoiX = slicedRect.x ;
    currRoiY = slicedRect.y ;  
}

// would work only with nvidia jetson. 
// tested with test stream. In irl scenerios we need to consider the 
// image format that we get. that is why we use videoconvert. it will autoconvert the image format that will fit to nvvidconv for example (CV_8c3u)RGB->RGBA.
// in that case manipulation need to be made becasue nvvidconv require RGBA/BGRA (CV_8UC4). we pass CV_8c3u -> into appsrc -> converting format videoconvert -> nnvidconv ->....

// void RTSPStreamer::initializeStream(int initialX, int initialY,int cameraInd)
// {

//     std::ostringstream pipeline;
//     pipeline << "appsrc ! videoconvert ! nvvidconv ! nvv4l2h264enc bitrate=" << bitrate << " ! h264parse ! rtspclientsink location=rtsp://localhost:8554/" << cameraInd ;
//     // Open the GStreamer pipeline with the specified parameters.
//     writer.open(pipeline.str(), 0, this->fps, cv::Size(this->roi_width,this->roi_height), true);
//     // Opens a GStreamer pipeline for video streaming.
//     this->set_roi(initialX,initialY);
//     // Error handling if the pipeline cannot be opened.
//     if (!writer.isOpened()) {
//         std::cerr << "Error: Could not open GStreamer pipeline." << std::endl;
//         throw std::runtime_error("Failed to open GStreamer pipeline");
//     }
//     this->streaming = true;
// }

// will work with any pc using CPU
// using the default libx264-based H.264 video encoder
void RTSPStreamer::initializeStream(int initialX, int initialY,int cameraInd)
{
    
    std::ostringstream pipeline;
    pipeline << "appsrc ! videoconvert ! x264enc bitrate=" << bitrate << " ! h264parse ! rtspclientsink location=rtsp://localhost:8554/" << cameraInd;
    // Open the GStreamer pipeline with the specified parameters.
    writer.open(pipeline.str(), 0, this->fps, cv::Size(this->roi_width,this->roi_height), true);
    // Opens a GStreamer pipeline for video streaming.
    this->set_roi(initialX,initialY);
    // Error handling if the pipeline cannot be opened.
    if (!writer.isOpened()) {
        std::cerr << "Error: Could not open GStreamer pipeline." << std::endl;
        throw std::runtime_error("Failed to open GStreamer pipeline");
    }
    this->streaming = true;
}



// Starts the streaming process in a separate thread.
void RTSPStreamer::start_stream() {
    this->streamThread = std::thread(&RTSPStreamer::stream, this);
    this->streamThread.detach();
}

//Stops the streams
void RTSPStreamer::stop_streaming() {
    {
        // std::lock_guard<std::mutex> lock(streamingMutex);
        streaming = false;
    }

    if (streamThread.joinable()) {
        streamThread.join();
    }
}

// Destructor: Joins the streaming thread if it is joinable.
RTSPStreamer::~RTSPStreamer() {
    this->stop_streaming();
    if (streamThread.joinable()) {
        streamThread.join();
    }
}



// Streaming function to be run in a separate thread.
void RTSPStreamer::stream() {
    while (true) {
        bool shouldContinue;
        {
            // std::lock_guard<std::mutex> lock(streamingMutex);
            shouldContinue = streaming;
        }

        if (!shouldContinue) {
            break;
        }
        // if (!streaming) 
        // {
        //     break;
        // }
        if (frame->empty()) {
            std::cerr << "Error: No frame captured." << std::endl;
            break;
        }

               // Slice the frame based on the selected region and write it to the stream.
        cv::Mat slicedFrame = (*frame)(slicedRect).clone();
        writer.write(slicedFrame);
    }
}

