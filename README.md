# Vivante
  Video player on Vivante GPU
  
--------------------
## Prerequisite
1. 필요 패키지 설치
```
  sudo apt-get install build-essential git unzip wget cmake libgtk2.0-dev pkg-config yasm 
  sudo apt-get install libjpeg-dev libtiff5-dev libpng-dev 
  sudo apt-get install ffmpeg libavcodec-dev libavformat-dev libswscale-dev libxvidcore-dev libx264-dev libxine2-dev 
  sudo apt-get install libgtk-3-dev  
  sudo apt-get install mesa-utils libgl1-mesa-dri libgtkgl2.0-dev libgtkglext1-dev 
  sudo apt-get install libatals-base-dev gfortran libeigen3-dev 
```

2. OpenCL 헤더 설치
```
  sudo apt-get install opencl-headers
```

3. FFMPEG 설치
```
  wget https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2 -O ffmpeg-snapshot.tar.bz2 
  mkdir libffmpeg 
  tar -xvf ffmpeg-snapshot.tar.bz2 -C libffmpeg --strip-components=1 
  rm ffmpeg-snapshot.tar.bz2 
  cd libffmpeg 
  ./configure --enable-shared –disable-doc 
  sudo make  
  sudo make install 
```

4. OpenCV 4.4 설치
```
  mkdir opencv
  cd opencv 
  wget –O opencv.zip https://github.com/opencv/opencv/archive/4.4.0.zip 
  unzip opencv.zip 
  wget –O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.4.0.zip 
  unzip opencv_contrib.zip 
```

```
  cd opencv-4.4.0 
  mkdir build 
  cd build 
  cmake -D CMAKE_BUILD_TYPE=RELEASE \ 
  -D CMAKE_C_COMPILER=/usr/bin/gcc \ 
  -D CMAKE_INSTALL_PREFIX=/usr/local \ 
  -D INSTALL_PYTHON_EXAMPLES=OFF \ 
  -D INSTALL_C_EXAMPLES=OFF \ 
  -D BUILD_DOCS=OFF \ 
  -D BUILD_PERF_TESTS=OFF \ 
  -D BUILD_TESTS=OFF \ 
  -D BUILD_PACKAGE=OFF \ 
  -D BUILD_EXAMPLES=OFF \ 
  -D WITH_TBB=ON \ 
  -D ENABLE_FAST_MATH=1 \ 
  -D CUDA_FAST_MATH=1 \ 
  -D WITH_IPP=OFF \ 
  -D WITH_V4L=OFF \ 
  -D WITH_1394=OFF \ 
  -D WITH_GTK=ON \ 
  -D WITH_QT=OFF \ 
  -D WITH_OPENGL=ON \ 
  -D WITH_EIGEN=ON \ 
  -D WITH_FFMPEG=ON \ 
  -D BUILD_JAVA=OFF \ 
  -D OPENCV_GENERATE_PKGCONFIG=ON \ 
  -D OPENCV_ENABLE_NONFREE=ON \ 
  -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.4.0/modules .. 
```
#### 주의: 결과에서 아래 그림과 같이 FFMPEG 지원을 반드시 확인하고 다음 단계로 넘어가야한다. 

```
  time make -j$(nproc)
  sudo make install
```

```
  cat /etc/ld.so.conf.d/*
```
결과에 /usr/local/lib가 있는지 확인하고, 없다면 추가한다.
```
  sudo ldconfig
```
