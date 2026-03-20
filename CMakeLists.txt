cmake_minimum_required(VERSION 3.16)
project(irgLab)

function(irg_add_app target_name)
    set(project_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    set(dependencies_dir "${project_dir}/../dependencies")

    file(GLOB dependency_sources CONFIGURE_DEPENDS
        "${dependencies_dir}/glad/src/glad.c"
    )
    file(GLOB_RECURSE project_headers CONFIGURE_DEPENDS
        "${project_dir}/headers/*.h"
        "${project_dir}/headers/*.hpp"
    )
    file(GLOB_RECURSE project_sources CONFIGURE_DEPENDS
        "${project_dir}/sources/*.cpp"
    )
    file(GLOB_RECURSE project_shaders CONFIGURE_DEPENDS
        "${project_dir}/shaders/*.comp"
        "${project_dir}/shaders/*.frag"
        "${project_dir}/shaders/*.geom"
        "${project_dir}/shaders/*.vert"
    )
    file(GLOB_RECURSE project_resources CONFIGURE_DEPENDS
        "${project_dir}/resources/*"
    )

    set(target_sources ${project_sources})

    if(NOT target_sources)
        set(placeholder_source "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_placeholder.cpp")
        file(WRITE "${placeholder_source}" "int main()\n{\n    return 0;\n}\n")
        set(target_sources "${placeholder_source}")
        set_source_files_properties("${placeholder_source}" PROPERTIES GENERATED TRUE)
    endif()

    add_executable(${target_name}
        ${target_sources}
        ${project_headers}
        ${project_shaders}
        ${dependency_sources}
        ${project_resources}
    )

    target_include_directories(${target_name} PRIVATE
        "${project_dir}/headers"
        "${dependencies_dir}/assimp/include"
        "${dependencies_dir}/glad/include"
        "${dependencies_dir}/glfw/include"
        "${dependencies_dir}/glm"
        "${dependencies_dir}/stb"
    )

    target_compile_definitions(${target_name} PRIVATE
        GLFW_INCLUDE_NONE
        PROJECT_SOURCE_DIR="${project_dir}"
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS})
        endif()
    endif()

    target_link_libraries(${target_name} PRIVATE assimp glfw)

    source_group(TREE "${project_dir}/headers" PREFIX "Header Files" FILES ${project_headers})
    source_group(TREE "${project_dir}/sources" PREFIX "Source Files" FILES ${project_sources})
    source_group(TREE "${project_dir}/shaders" PREFIX "Shader Files" FILES ${project_shaders})
    source_group(TREE "${project_dir}/resources" PREFIX "Resource Files" FILES ${project_resources})
    source_group(TREE "${dependencies_dir}" PREFIX "Dependencies" FILES ${dependency_sources})

    set_source_files_properties(${project_resources} PROPERTIES HEADER_FILE_ONLY TRUE)

    if(CMAKE_GENERATOR MATCHES "^Visual Studio")
        foreach(asset_file IN LISTS project_shaders project_resources)
            file(RELATIVE_PATH asset_relative_path "${project_dir}" "${asset_file}")
            string(REPLACE "/" "\\" asset_relative_path_vs "${asset_relative_path}")
            set_source_files_properties("${asset_file}" PROPERTIES
                VS_TOOL_OVERRIDE "Content"
                VS_SETTINGS "TargetPath=${asset_relative_path_vs}"
            )
        endforeach()
    endif()

    set(runtime_output_dir "${CMAKE_BINARY_DIR}/${target_name}")

    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY "${runtime_output_dir}"
    )

    set(asset_files ${project_shaders} ${project_resources})
    if(asset_files)
        set(asset_stamp "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_assets.stamp")
        set(asset_copy_commands)

        foreach(asset_file IN LISTS asset_files)
            file(RELATIVE_PATH asset_relative_path "${project_dir}" "${asset_file}")
            get_filename_component(asset_relative_dir "${asset_relative_path}" DIRECTORY)
            if(asset_relative_dir)
                list(APPEND asset_copy_commands
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                            "$<TARGET_FILE_DIR:${target_name}>/${asset_relative_dir}"
                )
            endif()
            list(APPEND asset_copy_commands
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${asset_file}"
                        "$<TARGET_FILE_DIR:${target_name}>/${asset_relative_path}"
            )
        endforeach()

        add_custom_command(
            OUTPUT "${asset_stamp}"
            ${asset_copy_commands}
            COMMAND ${CMAKE_COMMAND} -E touch "${asset_stamp}"
            DEPENDS ${asset_files}
            COMMENT "Syncing assets for ${target_name}"
            VERBATIM
        )

        set_source_files_properties("${asset_stamp}" PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
        target_sources(${target_name} PRIVATE "${asset_stamp}")
    endif()
endfunction()

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(irgLab/dependencies/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Treat assimp warnings as errors" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER TRUE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
add_subdirectory(irgLab/dependencies/assimp)

if(MSVC AND TARGET assimp)
    target_compile_options(assimp PRIVATE /WX-)
endif()

add_subdirectory(irgLab/primjerGLM)
add_subdirectory(irgLab/primjerASSIMP)
add_subdirectory(irgLab/primjerOpenGL)

add_subdirectory(irgLab/vjezba1)
add_subdirectory(irgLab/vjezba2)
add_subdirectory(irgLab/vjezba3)
add_subdirectory(irgLab/vjezba4)
add_subdirectory(irgLab/vjezba5a)
add_subdirectory(irgLab/vjezba5b)
add_subdirectory(irgLab/vjezba6)
add_subdirectory(irgLab/vjezba7)
add_subdirectory(irgLab/vjezba8a)
add_subdirectory(irgLab/vjezba8b)
add_subdirectory(irgLab/vjezba8c)
add_subdirectory(irgLab/vjezba9)
add_subdirectory(irgLab/vjezba10)
