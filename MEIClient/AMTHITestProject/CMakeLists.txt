# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2013-2019 Intel Corporation
cmake_minimum_required (VERSION 3.1)

add_executable (AMTHITestProject
  AMTHITestProject.cpp
)

target_link_libraries (AMTHITestProject
  gtest_main
  LmsMEIClient
)

install (TARGETS AMTHITestProject
  RUNTIME DESTINATION ${LMS_TESTDIR}
)
