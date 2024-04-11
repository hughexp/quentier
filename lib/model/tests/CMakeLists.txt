cmake_minimum_required(VERSION 3.5.1)

SET_POLICIES()

project(quentier_model_tests)

set(HEADERS
    modeltest.h
    TestMacros.h
    SavedSearchModelTestHelper.h
    TagModelTestHelper.h
    NotebookModelTestHelper.h
    NoteModelTestHelper.h
    FavoritesModelTestHelper.h
    ModelTester.h)

set(SOURCES
    modeltest.cpp
    SavedSearchModelTestHelper.cpp
    TagModelTestHelper.cpp
    NotebookModelTestHelper.cpp
    NoteModelTestHelper.cpp
    FavoritesModelTestHelper.cpp
    ModelTester.cpp
    )

# 使用指定的源文件来生成目标可执行文件。这里的目标可执行文件分为三类：普通可执行目标文件、导入可执行目标文件、别名可执行目标文件。分别对应上面的三种命令格式。
# 这里将测试文件生成一个可执行的文件，然后运行进行测试。
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  PREFIX ""
  CXX_STANDARD 14
  CXX_EXTENSIONS OFF)

add_sanitizers(${PROJECT_NAME})

#add_test(${PROJECT_NAME} ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} quentier_model ${THIRDPARTY_LIBS})

QUENTIER_COLLECT_HEADERS(HEADERS)
QUENTIER_COLLECT_SOURCES(SOURCES)
QUENTIER_COLLECT_INCLUDE_DIRS(${PROJECT_SOURCE_DIR})
