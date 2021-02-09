# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2010-2021 Intel Corporation

file (READ VERSION LMS_VERSION_STRING)
string (STRIP "${LMS_VERSION_STRING}" LMS_VERSION_STRING)
string (REPLACE "." ";" VER_LIST ${LMS_VERSION_STRING})
list (GET VER_LIST 0 LMS_VERSION_MAJOR)
list (GET VER_LIST 1 LMS_VERSION_MINOR)
list (GET VER_LIST 2 LMS_VERSION_PATCH)
list (GET VER_LIST 3 LMS_VERSION_BUILD)
string (TIMESTAMP LMS_VERSION_YEAR "%Y")

configure_file (
"${PROJECT_SOURCE_DIR}/version.h.in"
"${PROJECT_BINARY_DIR}/version.h"
)
