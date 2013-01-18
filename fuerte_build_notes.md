


### Undefined reference to boost\_system

#### Error

	/usr/bin/ld: CMakeFiles/stereoimageproc_exe.dir/src/nodes/stereo_image_proc.o: undefined reference to symbol '_ZN5boost6system15system_categoryEv'

#### Solution

To the CMakeLists.txt of the complaining ROS package, do one of the following, as appropriate:

* Add boost\_system to the target\_link\_libraries line.
* Add system to the rosbuild\_link\_boost line.


### PyString\_XXX not declared in this scope

#### Error

	/opt/ros/desktop-full/geometry/tf/src/pytf.cpp: In function ‘PyObject* asListOfStrings(std::vector<std::basic_string<char> >)’:
	/opt/ros/desktop-full/geometry/tf/src/pytf.cpp:224:60: error: ‘PyString_FromString’ was not declared in this scope
	/opt/ros/desktop-full/geometry/tf/src/pytf.cpp: In function ‘PyObject* setTransform(PyObject*, PyObject*)’:
	/opt/ros/desktop-full/geometry/tf/src/pytf.cpp:371:106: error: ‘PyString_AsString’ was not declared in this scope

#### Solution

Include the following in the appropriate CMakeLists.txt:

	set(PYTHON_INCLUDE_PATH /usr/include/python2.7)

