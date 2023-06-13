import os
from distutils.dir_util import copy_tree

os.system("git submodule init")
os.system("git submodule update")
os.system("cd modules/VulkanContext \n git submodule init \n git submodule update")


print(os.system("cmake --version"))
os.system("cd modulesToCompile/assimp \n mkdir build \n cd build\n cmake ../ \n cmake --build . --config Release -- -j "+str(os.cpu_count()))

os.system("mkdir NativeModules")
copy_tree("modulesToCompile/assimp/build/bin", "NativeModules/bin")
copy_tree("modulesToCompile/assimp/build/include", "NativeModules/include") 
copy_tree("modulesToCompile/assimp/include", "NativeModules/include")
copy_tree("modulesToCompile/assimp/contrib", "NativeModules/include/contrib")