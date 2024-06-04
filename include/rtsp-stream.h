#ifndef RTSPSTREAMER_H
#define RTSPSTREAMER_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>
#include <math.h>

#define ROI_DEFAULT_WIDTH 160
#define ROI_DEFAULT_HEIGHT 120
#define DEFAULT_BITRATE 40

/** 
 * @class RTSPStreamer
 * @brief This class is responsible for streaming video over RTSP using OpenCV and GStreamer.
 * 
 * The RTSPStreamer class enables the streaming of video frames, possibly with modifications such as
 * changing the region of interest (ROI), to an RTSP server. It allows for both basic and advanced 
 * initialization with adjustable frame rate, bitrate, and ROI dimensions.
 * @example main.cpp
 */



class RTSPStreamer {
public:
    /**
     * @brief Constructs an RTSPStreamer object with basic parameters.
     * @param framePtr Pointer to the OpenCV Mat object containing the frame to be streamed.
     * @param streaming Flag to start streaming immediately.
     */
    RTSPStreamer(cv::Mat* framePtr, bool streaming);

    /**
     * @brief Constructs an RTSPStreamer object with advanced streaming options.
     * @param framePtr Pointer to the OpenCV Mat object containing the frame to be streamed.
     * @param streaming Flag to start streaming immediately.
     * @param fps Frame rate of the video stream.
     * @param bitrate Bitrate of the video stream.
     * @param width Width of the region of interest (ROI).
     * @param height Height of the region of interest (ROI).
     */
    RTSPStreamer(cv::Mat* framePtr, bool streaming, double fps, int bitrate, int width, int height);

    /** 
     * @brief Destructor for the RTSPStreamer.
     */
    ~RTSPStreamer();
    
    /**
     * @brief Starts the video stream in a separate thread.
     */
    void start_stream();

    /**
     * @brief Sets the region of interest (ROI) for the video stream and center the position.
     * @param x The x-coordinate of the ROI's.
     * @param y The y-coordinate of the ROI's .
     */
    void set_roi(int x, int y);

    /**
     * @brief Initializes the video stream with specified ROI and camera index.
     * @param initialX Initial x-coordinate of the ROI.
     * @param initialY Initial y-coordinate of the ROI.
     * @param cameraInd Index of the camera for streaming.
     */
    void initializeStream(int initialX, int initialY, int cameraInd);

    /**
     * @brief Stops the video streaming process.
     */
    void stop_streaming();

private:
    void updateSlicedRect(int x, int y); ///< Helper function to update the ROI rectangle.
    void stream(); ///< Function that handles the streaming process.

    cv::Mat* frame; ///< Pointer to the frame to be streamed.
    int frameWidth, frameHeight; ///< Dimensions of the frame.
    cv::VideoWriter writer; ///< OpenCV VideoWriter for handling the video stream.
    cv::Rect slicedRect; ///< Rectangle representing the ROI.
    std::thread streamThread; ///< Thread for handling the streaming process.
    double fps; ///< Frame rate of the video stream.
    bool streaming; ///< Flag indicating whether streaming is active.
    std::mutex streamingMutex; ///< Mutex for thread-safe streaming state handling.
    int bitrate; ///< Bitrate of the video stream.
    int currRoiX, currRoiY; ///< Current x and y coordinates of the ROI.
    int roi_width, roi_height; ///< Width and height of the ROI.
};

#endif // RTSPSTREAMER_H
