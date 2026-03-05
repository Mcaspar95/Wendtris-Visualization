This program depends on:

📍 The matplotlibcpp.h header (copy into your include path)
📍 Python with matplotlib & NumPy installed
📍 Linking against the Python libraries

A typical compile command on Linux might look like:

g++ -std=c++11 program.cpp -I/path/to/python/include \
    -I/path/to/matplotlib-cpp \
    -L/path/to/python/libs -lpython3.10 -o delivery_sim

Adjust paths & Python version as necessary.
