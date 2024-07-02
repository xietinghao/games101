# Rotation and Translation
If compile main.cpp in your host, you need compile eigen from source code or install from packet manager tool such as apt, yum. Another way is compile main.cpp in virtual machine.You can get it from [the website](https://pan.baidu.com/s/198u7ETgY7h24IuYCYslmlw).
I implemented rotation and translation by Eigen::Matrix4f. In this homework, rotation around z axis can be implemented by matrix [cos(a),-sin(a),sin(a),cos(a)], in which a is rotation angle. And translation is implemented by homogeneous coordinate.
