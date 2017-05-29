QPP Libraries for Computational Chemistry
======
You compiler must support C++11 standard(g++>6.X).

We recommend to use out-of-source build as follows:
* Create temporary directory 
```
mkdir ./build
```

* Run cmake and then make
```
cd ./build
cmake .. 
make
```
Build flags
--

You can customize your build by using special configuration options:
```
cmake .. -D<Build Flag Name>=<Flag value>
```

|Build flag               |Description   							| Value Type(Default) |
|---			  |---           							|---		      |
|-DQPP_BUILD_EXAMPLES     |Build examples		             				|Bool(ON)	      |
|-DQPP_BUILD_DOC          |Build HTML documentation             				|Bool(OFF)	      |
|-DQPP_BUILD_QC     	  |Build quantum chemistry code   					|Bool(OFF)            |
|-DQPP_BUILD_TESTS     	  |Build tests executable	   					|Bool(OFF)            |
|-DQPP_USE_SPGLIB   	  |Use spglib for handling crystal symmetry    				|Bool(OFF)            |
|-DQPP_USE_STATIC_BOOST   |Linking with static boost   						|Bool(OFF)            |
|-DQPP_USE_SYSTEM_EIGEN3  |Use system Eigen3, instead download it   				|Bool(ON)             |
|-DQPP_PYTHON_BINDINGS    |Build python bindings		   				|Bool(ON)             |
|-DQPP_FORCE_PYTHON3      |Force to Build python 3.x bindings					|Bool(OFF)            |
|-DQPP_PYTHON3_LIBNAME    |Determine boost python library suffix (libboost_python-py**.a{so})	|String(35)           |
CI
--
* Travis CI OSX/Linux: [![Build Status](https://travis-ci.org/nvpopov/qpp.svg?branch=master)](https://travis-ci.org/nvpopov/qpp)

Docs
--
QPP C++ api : https://nvpopov.github.io/qppdoc/

