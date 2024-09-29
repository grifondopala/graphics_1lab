#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

void saveImage(Mat* image, string name) {
    imwrite("D:/МИФИ/KG/FirstLab/FirstLab/result/" + name, *image);
}

Mat loadImage(string name) {
    Mat image = imread("D:/МИФИ/KG/FirstLab/FirstLab/img/" + name);
    if (image.empty()) {
        cout << "Can not load image " << name << endl;
    }
    return image;
}

void showImage(Mat* image) {
    imshow("Display window", *image);
}

Mat createAlphaChannel(const Mat& image, float alphaValue) {
    Mat alpha(image.size(), CV_8UC1, Scalar(alphaValue * 255.0f));
    return alpha;
}

void grayInCircle() {
    Mat image = loadImage("moscow.jpg");

    Mat gray_image;
    cvtColor(image, gray_image, COLOR_BGR2GRAY);

    Mat mask(image.size(), CV_8UC1, Scalar(0));

    Point center(image.cols / 2, image.rows / 2);
    int radius = min(image.cols, image.rows) / 2;

    circle(mask, center, radius, Scalar(255), -1);

    Mat result;
    gray_image.copyTo(result, mask);

    Mat with_white_background(image.size(), CV_8UC1, Scalar(255));
    result.copyTo(with_white_background, mask);

    saveImage(&with_white_background, "moscowGray.jpg");
}

const string BlendMode[9] = {
    "normal", 
    "multiply", 
    "screen", 
    "darken", 
    "lighten", 
    "difference",
    "color_dodge",
    "color_burn",
    "soft_light",
};

float D(float x) {
    if (x <= 255.0f * 0.25) {
        return ((16 * x / 255.0f - 12) * x / 255.0f + 4) * x / 255.0f;
    }
    return sqrt(x / 255.0f);
}

float B(float Cb, float Cs, string mode) {
    if (mode == "normal") {
        return Cs;
    }
    if (mode == "multiply") {
        return Cs * Cb / 255.0f;
    }
    if (mode == "screen") {
        return (1 - (1 - Cs / 255.0f) * (1 - Cb / 255.0f)) * 255.0f;
    }
    if (mode == "darken") {
        return min(Cs, Cb);
    }
    if (mode == "lighten") {
        return max(Cs, Cb);
    }
    if (mode == "difference") {
        return abs(Cs - Cb);
    }
    if (mode == "color_dodge") {
        if (Cs == 255.0f) {
            return 255.0f;
        }

        return min(1.0f, Cb / (255.0f - Cs)) * 255.0f; 
    }
    if (mode == "color_burn") {
        if (Cs == 0) {
            return 0;
        }

        return (1 - min(1.0f, ( 255.0f - Cb ) / Cs )) * 255.0f;
    }

    if (mode == "soft_light") {
        if (Cs <= 255.0f * 0.5) {
            return (Cb / 255.0f - (1 - 2 * Cs / 255.0f) * Cb / 255.0f * (1 - Cb / 255.0f)) * 255.0f;
        }
        return (Cb / 255.0f + (2 * Cs / 255.0f - 1) * (D(Cb) - Cb / 255.0f)) * 255.0f;
    }

    return 0;
}


void blending(string mode) {
    Mat shaman = loadImage("shaman.jpg");
    Mat russia = loadImage("russia.jpg");

    Mat alpha = createAlphaChannel(shaman, 0.5f);
    Mat alpha1 = createAlphaChannel(russia, 0.8f);

    Mat result = Mat::zeros(shaman.size(), CV_8UC3);

    for (int y = 0; y < shaman.rows; y++) {
        for (int x = 0; x < shaman.cols; x++) {
            Vec3b Cs = shaman.at<Vec3b>(y, x);
            Vec3b Cb = russia.at<Vec3b>(y, x);
            float alphaS = alpha.at<uchar>(y, x) / 255.0f;
            float alphaB = alpha1.at<uchar>(y, x) / 255.0f;

            for (int c = 0; c < 3; c++) {
                result.at<Vec3b>(y, x)[c] = (1 - alphaS) * alphaB * Cb[c] + (1 - alphaB) * alphaS * Cs[c] + alphaS * alphaB * B(Cb[c], Cs[c], mode);
            }
        }
    }

    saveImage(&result, "shaman_" + mode + ".jpg");
}

void mirror() {
    Mat shaman = loadImage("shaman.jpg");

    for (int x = 0; x < shaman.cols / 2; x++) {
        for (int y = 0; y < shaman.rows; y++) {
            Vec3b leftPixel = shaman.at<Vec3b>(y, x);
            Vec3b rightPixel = shaman.at<Vec3b>(y, shaman.cols - x - 1);
            Vec3b copy = rightPixel;

            shaman.at<Vec3b>(y, shaman.cols - x - 1) = leftPixel;
            shaman.at<Vec3b>(y, x) = copy;
        }
    }

    saveImage(&shaman, "shaman_mirror.jpg");
}

void transpon() {
    Mat shaman = loadImage("shaman.jpg");

    Mat result = Mat::zeros(shaman.cols, shaman.rows, CV_8UC3);

    for (int y = 0; y < shaman.rows; y++) {
        for (int x = 0; x < shaman.cols; x++) {
            result.at<Vec3b>(x, y) = shaman.at<Vec3b>(y, x);
        }
    }

    saveImage(&result, "shaman_transpon.jpg");
}

int main() {
    grayInCircle();

    for (int i = 0; i < size(BlendMode); i++) {
        blending(BlendMode[i]);
    }

    mirror();

    transpon();

    waitKey(0);
    return 0;
}



