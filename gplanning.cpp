#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;
void on_mouse(int event, int x, int y, int flags, void* dstImg) {
    static int count;
    int a, b, cnt = 0;
    double sum;

    Mat img = *(Mat*)dstImg;

    Mat Kernel = getStructuringElement(MORPH_RECT, Size(10, 10));
    Mat erode_image;
    erode(img, erode_image, Kernel);

    cvtColor(img, img, COLOR_BGR2GRAY);

    cvtColor(erode_image, erode_image, COLOR_BGR2GRAY);

    static Point ptOld, pt;

    static vector <Point> Close_List;

    vector <Point> Open_List;

    vector <double> total;

    vector <int> cross_x = { 1,-1,0,0 };
    vector <int> cross_y = { 0,0,1,-1 };
    vector <int> diagonal_x = { 1,-1,-1,1 };
    vector <int> diagonal_y = { 1,1,-1,-1 };

    if (count == 0 && event == EVENT_LBUTTONDOWN) {
        ptOld = Point(x, y);

        cout << "ptOld(x,y): " << ptOld << endl;
        Close_List.push_back(ptOld);
        //cout << (int)img.at<uchar>(y, x) << endl;
        count++;
    }
    else if (count == 1 && event == EVENT_LBUTTONDOWN) {
        pt = Point(x, y);
        cout << "pt" << pt;

        while (1) {
            cout << "Close_List: " << Close_List[cnt] << endl;

            for (int i = 0; i < 4; i++) {  //닫힌 목록 주변에 십자가 모양의 칸에 장애물이 있으면 
                if ((int)erode_image.at<uchar>(Close_List[cnt].y + cross_y[i], // 열린 목록에 못들어가도록 설정
                    Close_List[cnt].x + cross_x[i]) > 253) {
                    Point cpt = Point(Close_List[cnt].x + cross_x[i], Close_List[cnt].y + cross_y[i]);
                    if (cnt > 0 && cpt == Close_List[cnt - 1]) continue;
                    Open_List.push_back(cpt);
                }
            }
            for (int j = 0; j < 4; j++) {  //닫힌 목록 주변에 대각선 칸에 장애물이 있으면 
                if ((int)erode_image.at<uchar>(Close_List[cnt].y + diagonal_y[j], //열린 목록에 못들어가도록 설정
                    Close_List[cnt].x + diagonal_x[j]) > 253) {
                    Point cpt = Point(Close_List[cnt].x + diagonal_x[j], Close_List[cnt].y + diagonal_y[j]);
                    if (cnt > 0 && cpt == Close_List[cnt - 1]) continue;
                    Open_List.push_back(cpt);
                }
            }
            //cout << "Open_List size: " << Open_List.size()<<endl;

            // g값 + h값
            if (Open_List.size() == 8) {
                for (int k = 0; k < Open_List.size(); k++) {
                    a = Open_List[k].x - Close_List[cnt].x;
                    b = Open_List[k].y - Close_List[cnt].y;

                    if (cross_x[k % 4] == a && cross_y[k % 4] == b) {
                        sum = 1.0 + abs(pt.x - (Close_List[cnt].x + a)) + abs(pt.y - (Close_List[cnt].y + b));
                        total.push_back(sum);
                        sum = 0;
                    }
                    else if (diagonal_x[k % 4] == a && diagonal_y[k % 4] == b) {
                        sum = 1.4 + abs(pt.x - (Close_List[cnt].x + a)) + abs(pt.y - (Close_List[cnt].y + b));
                        total.push_back(sum);
                        sum = 0;
                    }
                    cout << "total: " << total[k] << endl;
                }
            }
            else {
                cout << "Open_List size: " << Open_List.size() << endl;
                for (int k = 0; k < Open_List.size(); k++) {
                    a = Open_List[k].x - Close_List[cnt].x;
                    b = Open_List[k].y - Close_List[cnt].y;

                    for (int i = 0; i < 4; i++) {
                        if (cross_x[i] == a && cross_y[i] == b) {
                            sum = 1.0 + abs(pt.x - (Close_List[cnt].x + a)) + abs(pt.y - (Close_List[cnt].y + b));
                            total.push_back(sum);
                            sum = 0;
                        }
                    }
                    for (int i = 0; i < 4; i++) {
                        if (diagonal_x[i] == a && diagonal_y[i] == b) {
                            sum = 1.4 + abs(pt.x - (Close_List[cnt].x + a)) + abs(pt.y - (Close_List[cnt].y + b));
                            total.push_back(sum);
                            sum = 0;
                        }
                    }
                    cout << "total: " << total[k] << endl;
                }
            }

            //f=g+h 합이 제일 작은 위치를 찾아낸다. 그것을 닫힌 목록에 저장한다.

            double value1 = total[0];
            int num = 0;
            for (int i = 1; i < Open_List.size(); i++) {
                if (pt.y > ptOld.y) {
                    if (value1 >= total[i]) {
                        value1 = total[i];
                        num = i;
                    }
                }
                else {
                    if (value1 > total[i]) {
                        value1 = total[i];
                        num = i;
                    }
                }
            }
            cout << "num: " << num << endl;
            cout << "좌표: " << Open_List[num] << endl;

            Close_List.push_back(Open_List[num]);
            Open_List.erase(Open_List.begin(), Open_List.end());
            total.erase(total.begin(), total.end());
            cnt++;

            if (Close_List[cnt] == pt) break;
        }
        cvtColor(img, img, COLOR_GRAY2BGR);
        cvtColor(erode_image, erode_image, COLOR_GRAY2BGR);

        for (int j = 0; j < img.rows; j++) {
            for (int i = 0; i < img.cols; i++) {
                Vec3b image_pixel = img.at<Vec3b>(j, i);
                if (image_pixel[0] >= 240 && image_pixel[1] >= 240 &&
                    image_pixel[2] >= 240 && erode_image.at<Vec3b>(j, i) == Vec3b(0, 0, 0)) {
                    img.at<Vec3b>(j, i) = Vec3b(255, 153, 0);
                }
            }
        }
        circle(img, ptOld, 3, Scalar(0, 255, 0));
        circle(img, pt, 3, Scalar(255, 0, 0));
        for (int k = 0; k < Close_List.size(); k++) {
            circle(img, Close_List[k], 0.5, Scalar(0, 0, 255));
        }
        imshow("img", img);

        count = 0;
    }

}
int main() {
    Mat src = imread("map.png");

    Mat dst = src.clone();

    if (src.empty()) {
        cerr << "image load fail!" << endl;
        return -1;
    }
    /*for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            if ((int)src.at<uchar>(j, i) > 0 && (int)src.at<uchar>(j, i) < 205) {
                src.at<uchar>(j, i) = 0;
            }
            else if ((int)src.at<uchar>(j, i) > 205 && (int)src.at<uchar>(j, i) < 254) {
                src.at<uchar>(j, i) = 255;
            }
        }
    */

    namedWindow("map");
    setMouseCallback("map", on_mouse, (void*)&src);

    imshow("map", src);
    waitKey();

    return 0;
}
