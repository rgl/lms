/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2013-2015 Intel Corporation
 */
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl COMEVENTHANDLER
// ------------------------------
#ifndef COMEVENTHANDLER_EXPORT_H
#define COMEVENTHANDLER_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (COMEVENTHANDLER_HAS_DLL)
#  define COMEVENTHANDLER_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && COMEVENTHANDLER_HAS_DLL */

#if !defined (COMEVENTHANDLER_HAS_DLL)
#  define COMEVENTHANDLER_HAS_DLL 1
#endif /* ! COMEVENTHANDLER_HAS_DLL */

#if defined (COMEVENTHANDLER_HAS_DLL) && (COMEVENTHANDLER_HAS_DLL == 1)
#  if defined (COMEVENTHANDLER_BUILD_DLL)
#    define COMEVENTHANDLER_Export ACE_Proper_Export_Flag
#    define COMEVENTHANDLER_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define COMEVENTHANDLER_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* COMEVENTHANDLER_BUILD_DLL */
#    define COMEVENTHANDLER_Export ACE_Proper_Import_Flag
#    define COMEVENTHANDLER_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define COMEVENTHANDLER_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* COMEVENTHANDLER_BUILD_DLL */
#else /* COMEVENTHANDLER_HAS_DLL == 1 */
#  define COMEVENTHANDLER_Export
#  define COMEVENTHANDLER_SINGLETON_DECLARATION(T)
#  define COMEVENTHANDLER_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* COMEVENTHANDLER_HAS_DLL == 1 */

// Set COMEVENTHANDLER_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (COMEVENTHANDLER_NTRACE)
#  if (ACE_NTRACE == 1)
#    define COMEVENTHANDLER_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define COMEVENTHANDLER_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !COMEVENTHANDLER_NTRACE */

#if (COMEVENTHANDLER_NTRACE == 1)
#  define COMEVENTHANDLER_TRACE(X)
#else /* (COMEVENTHANDLER_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define COMEVENTHANDLER_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (COMEVENTHANDLER_NTRACE == 1) */

#endif /* COMEVENTHANDLER_EXPORT_H */

// End of auto generated file.
