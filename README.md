## Shared_Cache Lib
This branch encapsulates the project as a static/dynamic library. It trims the header files in **include** directory to support be embedded into Cedar on G++4.4.7

#### SC Compile
##### Libs
+ G++ 4.9 or higher on CentOS 7
+ RDMA libraries
+ cmake 2.8 or higher

##### CMakeLists.txt
    set(SRC_NET ${PROJECT_SOURCE_DIR}/src/net)
    set(SRC_INDEX ${PROJECT_SOURCE_DIR}/src/index)
    set(SRC_CLIENT ${PROJECT_SOURCE_DIR}/src/client)
    set(SRC_SERVER ${PROJECT_SOURCE_DIR}/src/server)
    
define file pathes

    file(GLOB_RECURSE NET_LIB ${SRC_NET}/*.cpp)
    file(GLOB_RECURSE INDEX_LIB ${SRC_INDEX}/*.cpp)
    file(GLOB_RECURSE CLIENT_LIB ${SRC_CLIENT}/*.cpp)
    file(GLOB_RECURSE SERVER_LIB ${SRC_SERVER}/*.cpp)
    
take source file in the file pathes to generate a static/dynamic lib 

    add_library(RDMAClientStatic ${NET_LIB} ${INDEX_LIB} ${CLIENT_LIB} ${SERVER_LIB}) # static lib
    add_library(RDMAClientStatic ${NET_LIB} SHARED ${INDEX_LIB} ${CLIENT_LIB} ${SERVER_LIB}) # dynamic lib
    

#### Cedar Compile
##### Libs
+ compact-g++44/compact-gcc44
+ libstdc++.so.6.0.20
+ libboost-devel

##### Configure
    ./configure CXX=/usr/bin/g++44 CC=/usr/bin/gcc44 --prefix=/home/admin/oceanbase_install --with-release=yes --with-test-case=no;

give the specified compilers in `./configure`

##### Complie Link
+ build a directory to store all related header files and the static lib **(SCLib)**

      ├── oceanbase/
         ├── src/
            ├── SCLib
               ├── libRDMAClientStatic.a
               ├── client.h
               ├── common.hpp
               ├── debug.hpp
               ├── RdmaSocket.hpp
               ├── BlockBitmap.h
               ├── Configuration.gpp
               ├── ....


+ add SCLib into the MakeFile.am in src directory

      SUBDIRS=sql common obmysql sstable compactsstable compactsstablev2 rootserver mergeserver chunkserver updateserver compactsstable lsync client proxyserver importserver mrsstable SCLib




+ add libRDMAClientStatic.a into the MakeFile.am in sql directory

      LDADD = ${top_srcdir}/src/mergeserver/libmergeserver.a \
              ${top_srcdir}/src/sstable/libsstable.a         \
              ${ONEV_LIB_PATH}/libonev.a                     \
              ${TBLIB_ROOT}/lib/libtbsys.a                   \
              ${top_srcdir}/src/SCLib/libRDMAClientStatic.a

+ do `./configure` again

+ add header file in SCLib into target source files `#include "SCLib/client.h"`

+ make -j all & make install

