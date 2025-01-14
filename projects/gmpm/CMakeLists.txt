cmake_minimum_required(VERSION 3.19)

#set(ENABLE_GMPM_BUILD ON)
set(ENABLE_GMPM_BUILD ON)

if (ENABLE_GMPM_BUILD)

project(MPM CXX CUDA)
set(ZS_ENABLE_CUDA ON)
set(ZS_ENABLE_OPENVDB ON)
set(ZENSIM_INSTALL OFF)
add_subdirectory(zecomp)

#if (EXISTS zecomp AND EXISTS zecomp/include/zensim/tpls)

#message("enable gmpm:${ENABLE_GMPM_BUILD}, cuda compiler: ${CMAKE_CUDA_COMPILER}")

# if (POLICY CMP0104)
#   cmake_policy(SET CMP0104 OLD)
# endif (POLICY CMP0104)

add_library(gmpm SHARED 
  nosys/ToZensimParticles.cpp 
  nosys/OutputZensimParticles.cpp 
  nosys/ConstitutiveModels.cpp 
  nosys/ToZensimLevelSet.cpp 
  nosys/ZensimBoundary.cpp 
  nosys/MakePartitionAndGrid.cpp 
  nosys/MakeIndexBuckets.cpp 

  nosys/ZensimParticleWrangler.cu
  nosys/ZensimParticleNeighborWrangler.cu
  #nosys/Test.cu
  nosys/PartitionForParticles.cu 
  nosys/GridFromPartition.cu 
  nosys/TransferNodes.cu 
  nosys/GridOps.cu 
  nosys/ToParticleObject.cpp 
  nosys/AppendZensimObject.cpp)

if (${ZS_ENABLE_CUDA})
    enable_language(CUDA)
    target_compile_features(gmpm PRIVATE cuda_std_17)
endif()
target_compile_features(gmpm PRIVATE cuda_std_17)

target_compile_features(gmpm PRIVATE cxx_std_17)

#simd compiler flags
if (WIN32)
else()
target_compile_options(gmpm PRIVATE "-march=native")
endif()
target_compile_options(gmpm 
      PRIVATE        $<$<COMPILE_LANGUAGE:CUDA>:${CMAKE_CUDA_FLAGS} -dlto -fopenmp --expt-extended-lambda --expt-relaxed-constexpr --default-stream=per-thread --use_fast_math -lineinfo --ptxas-options=-allow-expensive-optimizations=true>
    )
target_link_options(gmpm 
      PUBLIC       $<$<LINK_LANGUAGE:CUDA>:-dlto>
    )
set_target_properties(gmpm
      PROPERTIES  CUDA_EXTENSIONS ON
                  CUDA_SEPARABLE_COMPILATION ON
                  LINKER_LANGUAGE CUDA
                  POSITION_INDEPENDENT_CODE ON
                  CUDA_ARCHITECTURES OFF
    )

get_property(zeno_AUTOLOAD_DIR TARGET zeno PROPERTY LIBRARY_OUTPUT_DIRECTORY)
set_target_properties(gmpm PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${zeno_AUTOLOAD_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${zeno_AUTOLOAD_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${zeno_AUTOLOAD_DIR}
	LIBRARY_OUTPUT_DIRECTORY_DEBUG ${zeno_AUTOLOAD_DIR}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${zeno_AUTOLOAD_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${zeno_AUTOLOAD_DIR}
	LIBRARY_OUTPUT_DIRECTORY_RELEASE ${zeno_AUTOLOAD_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${zeno_AUTOLOAD_DIR}
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${zeno_AUTOLOAD_DIR}
	)
target_link_libraries(gmpm PRIVATE zensim)
target_link_libraries(gmpm PRIVATE zeno)
target_link_libraries(gmpm PUBLIC ZFX)
target_include_directories(gmpm PRIVATE ../oldzenbase/include)
target_include_directories(gmpm PRIVATE ../zenvdb/include)

endif(ENABLE_GMPM_BUILD)
