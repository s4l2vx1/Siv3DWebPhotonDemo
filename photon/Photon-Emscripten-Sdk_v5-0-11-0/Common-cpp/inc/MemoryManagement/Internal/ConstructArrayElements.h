/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Helpers/TypeTraits/EnableIf.h"
#include "Common-cpp/inc/Helpers/TypeTraits/IsPrimitiveType.h"

#if defined _EG_EMSCRIPTEN_PLATFORM || defined __arm___ || defined _M_ARM
 // Emscripten requires arrays of objects that have member variables of type double to be 8 byte aligned
 // 32bit ARM architectures require 64bit std::atomic types to be 8 byte aligned for access to them to actually be atomic
 // both is only the case when storing such data in memory that was allocated through allocateArray()/ALLOCATE_ARRAY, when we use 8 bytes instead of 4 to store the element count
#	define EG_SIZE_T unsigned long long
#else
#	define EG_SIZE_T size_t
#endif

namespace ExitGames
{
	namespace Common
	{
		namespace MemoryManagement
		{
			namespace Internal
			{
#				define CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, ...)                      for(EG_SIZE_T i=0; i<count; ++i) ::new(p+i) Ftype(__VA_ARGS__);
#				define CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(p, count, initVal)     MEMSET(p, initVal, count*sizeof(Ftype));
#				define COPY_CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(pOut, count, pIn)              for(EG_SIZE_T i=0; i<count; ++i) ::new(pOut+i) Ftype(pIn[i]);
#				define COPY_CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(pOut, count, pIn) MEMCPY(pOut, pIn, count*sizeof(Ftype));

				template<typename Ftype>                                                                  typename Helpers::EnableIf<!Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count)                                                                       {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count)}
				template<typename Ftype>                                                                  typename Helpers::EnableIf< Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count)                                                                       {CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(p, count, 0)}
				template<typename Ftype, typename P1>                                                     typename Helpers::EnableIf<!Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1)                                                         {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1)}
				template<typename Ftype, typename P1>                                                     typename Helpers::EnableIf< Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1)                                                         {CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(p, count, p1)}
				template<typename Ftype, typename P1>                                                     typename Helpers::EnableIf<!Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1)                                                         {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1)}
				template<typename Ftype, typename P1>                                                     typename Helpers::EnableIf< Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1)                                                         {CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(p, count, p1)}
				template<typename Ftype>                                                                  typename Helpers::EnableIf<!Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count,       Ftype* pIn)                                                     {COPY_CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, pIn)}
				template<typename Ftype>                                                                  typename Helpers::EnableIf< Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count,       Ftype* pIn)                                                     {COPY_CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(p, count, pIn)}
				template<typename Ftype>                                                                  typename Helpers::EnableIf<!Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count, const Ftype* pIn)                                                     {COPY_CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, pIn)}
				template<typename Ftype>                                                                  typename Helpers::EnableIf< Helpers::IsPrimitiveType<Ftype>::is>::type constructArrayElements(Ftype* p, EG_SIZE_T count, const Ftype* pIn)                                                     {COPY_CONSTRUCT_ARRAY_ELEMENTS_IS_PRIMITIVE_TEMPLATE_BODY(p, count, pIn)}
				template<typename Ftype, typename P1, typename P2>                                        void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2)                                           {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2)}
				template<typename Ftype, typename P1, typename P2>                                        void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2)                                           {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2)}
				template<typename Ftype, typename P1, typename P2>                                        void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2)                                           {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2)}
				template<typename Ftype, typename P1, typename P2>                                        void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2)                                           {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3>                           void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3)                             {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3,       P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4>              void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3, const P4& p4)               {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2,       P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1,       P2& p2, const P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2,       P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count,       P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2,       P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1,       P2& p2, const P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2,       P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3,       P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3,       P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3, const P4& p4,       P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}
				template<typename Ftype, typename P1, typename P2, typename P3, typename P4, typename P5> void                                                                   constructArrayElements(Ftype* p, EG_SIZE_T count, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5) {CONSTRUCT_ARRAY_ELEMENTS_TEMPLATE_BODY(p, count, p1, p2, p3, p4, p5)}

				template<typename Ftype> typename Helpers::EnableIf<!Helpers::IsPrimitiveType<Ftype>::is>::type destructArrayElements(Ftype* p, EG_SIZE_T count) {for(EG_SIZE_T i=count; i-->0;) p[i].~Ftype();}
				template<typename Ftype> typename Helpers::EnableIf< Helpers::IsPrimitiveType<Ftype>::is>::type destructArrayElements(Ftype*  , EG_SIZE_T)       {}
			}
		}
	}
}