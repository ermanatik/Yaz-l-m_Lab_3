#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>

void applyGaussianBlur(cv::Mat& image, int startRow, int endRow, int kernelSize) {

    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                float sum = 0.0f;
                for (int m = -kernelSize; m <= kernelSize; ++m) {
                    for (int n = -kernelSize; n <= kernelSize; ++n) {
                        int row = std::min(std::max(i + m, 0), image.rows - 1);
                        int col = std::min(std::max(j + n, 0), image.cols - 1);
                        sum += image.at<cv::Vec3b>(row, col)[c];
                    }
                }
                image.at<cv::Vec3b>(i, j)[c] = static_cast<uchar>(sum / ((2 * kernelSize + 1) * (2 * kernelSize + 1)));
            }
        }
    }
}

void applyGaussianBlurSingleThread(cv::Mat& image, int kernelSize) {
    applyGaussianBlur(image, 0, image.rows, kernelSize);
}

void blurImage(cv::Mat& image, int numThreads, int kernelSize, bool multiThreaded) {

    if (multiThreaded != 0 && multiThreaded != 1) {
        std::cerr << "Hata: Coklu iS parcacigi tercihi icin gecersiz girdi. Lutfen evet için 1, hayir icin 0 girin." << std::endl;
        return;
    }


    auto startTime = std::chrono::high_resolution_clock::now();

    if (multiThreaded) {
        // Görüntüyü çoklu iş parçacığı için parçalara bölme
        std::vector<std::thread> threads;
        int chunkSize = image.rows / numThreads;
        for (int i = 0; i < numThreads; ++i) {
            int startRow = i * chunkSize;
            int endRow = (i == numThreads - 1) ? image.rows : (i + 1) * chunkSize;
            threads.emplace_back(applyGaussianBlur, std::ref(image), startRow, endRow, kernelSize);
        }


        for (auto& thread : threads) {
            thread.join();
        }
    }
    else {
        // Bulanıklaştırma işlemi için tek bir iş parçacığı kullanma
        applyGaussianBlurSingleThread(image, kernelSize);
    }


    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);


    std::cout << "Goruntu Boyutlari: " << image.cols << "x" << image.rows << std::endl;
    std::cout << "Calisma Suresi: " << duration.count() << " milisaniye" << std::endl;


    cv::imwrite("output_blurred_image_2.jpg", image);
}

int main() {

    cv::Mat image = cv::imread("C:/Users/ERMAN/Desktop/indir.jpeg");
    if (image.empty()) {
        std::cerr << "Hata: Goruntu okunamiyor." << std::endl;
        return 1;
    }


    int numThreads;
    std::cout << "is parcacigi sayisini girin: ";
    std::cin >> numThreads;

    int kernelSize;
    std::cout << "Cekirdek boyutunu girin: ";
    std::cin >> kernelSize;

    // Kullanıcıya çoklu iş parçacığı tercihini sorma
    int multiThreaded;
    std::cout << "coklu is parcacigi kullanmak istiyor musunuz? (Evet icin 1, hayir icin 0): ";
    std::cin >> multiThreaded;


    blurImage(image, numThreads, kernelSize, multiThreaded);

    return 0;
}
