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

Mat createGrayscale(const Mat& image) {
    Mat gray_image;
    cvtColor(image, gray_image, COLOR_BGR2GRAY);

    return gray_image;
}

void grayInCircle() {
    Mat image = loadImage("moscow.jpg");

    Mat gray_image = createGrayscale(image);

    Mat mask(image.size(), CV_8UC1, Scalar(0));

    Point center(image.cols / 2, image.rows / 2);
    int radius = min(image.cols, image.rows) / 2;

    circle(mask, center, radius, Scalar(255), -1);

    Mat result(image.size(), CV_8UC4);

    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            if (mask.at<uchar>(y, x) == 255) {
                uchar color = gray_image.at<uchar>(y, x);
                result.at<Vec4b>(y, x) = Vec4b(color, color, color, 255);
            }
            else {
                result.at<Vec4b>(y, x) = Vec4b(0, 0, 0, 0);
            }
        }
    }

    saveImage(&result, "moscowGray.png");
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
    if (x <= 0.25f) {
        return ((16 * x - 12) * x + 4) * x;
    }
    return sqrt(x);
}

float B(float Cb, float Cs, string mode) {
    if (mode == "normal") {
        return Cs;
    }
    if (mode == "multiply") {
        return Cs * Cb;
    }
    if (mode == "screen") {
        return (1 - (1 - Cs) * (1 - Cb));
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
        if (Cs == 1.0f) {
            return 1.0f;
        }

        return min(1.0f, Cb / (1.0f - Cs)); 
    }
    if (mode == "color_burn") {
        if (Cs == 0) {
            return 0;
        }

        return (1 - min(1.0f, ( 1.0f - Cb ) / Cs ));
    }

    if (mode == "soft_light") {
        if (Cs <= 0.5f) {
            return (Cb - (1 - 2 * Cs) * Cb * (1 - Cb));
        }
        return (Cb + (2 * Cs - 1) * (D(Cb) - Cb));
    }

    return 0;
}


void blending(string mode) {
    Mat shaman  = loadImage("shaman.jpg");
    Mat russia  = loadImage("russia.jpg");
    Mat city    = loadImage("city.jpg");
    Mat sunrise = loadImage("sunrise.jpg");

    Mat cityGray     = createGrayscale(city);
    Mat sunriseGray  = createGrayscale(sunrise);

    Mat result  = Mat::zeros(shaman.size(), CV_8UC3);

    for (int y = 0; y < shaman.rows; y++) {
        for (int x = 0; x < shaman.cols; x++) {
            Vec3b CsP    = shaman.at<Vec3b>(y, x);
            Vec3b CbP    = russia.at<Vec3b>(y, x);
            float alphaS = cityGray.at<uchar>(y, x)    / 255.0f;
            float alphaB = sunriseGray.at<uchar>(y, x) / 255.0f;

            for (int c = 0; c < 3; c++) {
                float Cs = CsP[c] / 255.0f;
                float Cb = CbP[c] / 255.0f;
                result.at<Vec3b>(y, x)[c] = ((1 - alphaS) * alphaB * Cb + (1 - alphaB) * alphaS * Cs + alphaS * alphaB * B(Cb, Cs, mode)) * 255.0f;
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



