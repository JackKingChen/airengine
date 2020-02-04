//****************************************************************************//
//*	作者	:  HJJ                                                           
//*	日期    :  2008-12-9							                 
//* 标题    :  SIMD.h                                                 
//* 功能    :                                                          
//* 说明    :  Copyright (C) 2008 - All Rights Reserved                    
//****************************************************************************//


#ifndef  __SIMD_INCLUDE__
#define  __SIMD_INCLUDE__

#pragma once
//-------------------------------------------------------------
// 添加头文件
//-------------------------------------------------------------
#include <mmintrin.h>
#include <emmintrin.h>


#define  SHUFFLE(r0, r1, r2, r3)	(((r3) << 6) | ((r2) << 4) | ((r1) << 2) | (r0))

//////////////////////////////////////////////////////
// 但指令多数据流数据类型定义:						//
//////////////////////////////////////////////////////
typedef __m64				v8qi;
typedef __m64				v4hi;
typedef __m64				v2si;
typedef __m64				di;

typedef __m128i				v4si;
typedef __m128				v4sf;

typedef __m128d				v2df;
typedef __m128i				v2di;
typedef __m128i				v8hi;
typedef __m128i				v16qi;
//--------------------------------------------------//

#define INST(inst)			_m_##inst
#define SINSTC(inst, s)		_m_##inst##s
#define SINSTC2(inst, s)	_mm_##inst##s
#define SINST(inst, s)		SINSTC2(inst,_)##s
#define CINST(inst)			SINSTC2(inst,_ss)
#define CINST2(inst, cmp)	SINSTC2(inst##cmp,_sd)
#define INSTI(inst)			SINSTC(inst, i)

#define emms()				_m_empty()
#define MOVE				move
#define MOVEHL				movehl
#define MOVELH				movelh
#define MOVEMASK			movemask
#define UNPACKHI			unpackhi
#define UNPACKLO			unpacklo

// MMX arithmetic ops
#define paddb				INST(paddb)
#define paddw				INST(paddw)
#define paddd				INST(paddd)
#define psubb				INST(psubb)
#define psubw				INST(psubw)
#define psubd				INST(psubd)
#define paddsb				INST(paddsb)
#define paddsw				INST(paddsw)
#define psubsb				INST(psubsb)
#define psubsw				INST(psubsw)
#define paddusb				INST(paddusb)
#define paddusw				INST(paddusw)
#define psubusb				INST(psubusb)
#define psubusw				INST(psubusw)
#define pmullw				INST(pmullw)
#define pmulhw				INST(pmulhw)

// MMX logical ops
#define pand				INST(pand)
#define pandn				INST(pandn)
#define por					INST(por)
#define pxor				INST(pxor)
#define psllwv				INST (psllw)
#define psllwi				INSTI(psllw)
#define pslldv				INST (pslld)
#define pslldi				INSTI(pslld)
#define psllqv				INST (psllq)
#define psllqi				INSTI(psllq)
#define psrawv				INST (psraw)
#define psrawi				INSTI(psraw)
#define psradv				INST (psrad)
#define psradi				INSTI(psrad)
#define psrlwv				INST (psrlw)
#define psrlwi				INSTI(psrlw)
#define psrldv				INST (psrld)
#define psrldi				INSTI(psrld)
#define psrlqv				INST (psrlq)
#define psrlqi				INSTI(psrlq)

// MMX comparison ops
#define pcmpeqb				INST(pcmpeqb)
#define pcmpeqw				INST(pcmpeqw)
#define pcmpeqd				INST(pcmpeqd)
#define pcmpgtb				INST(pcmpgtb)
#define pcmpgtw				INST(pcmpgtw)
#define pcmpgtd				INST(pcmpgtd)

// MMX pack ops
#define punpckhbw			INST(punpckhbw)
#define punpckhwd			INST(punpckhwd)
#define punpckhdq			INST(punpckhdq)
#define punpcklbw			INST(punpcklbw)
#define punpcklwd			INST(punpcklwd)
#define punpckldq			INST(punpckldq)
#define packsswb			INST(packsswb)
#define packssdw			INST(packssdw)
#define packuswb			INST(packuswb)

// MMX misc ops
#define setzerodi			_mm_setzero_si64

// SSE arithmetic ops
#define addps				SINST(add, ps)
#define subps				SINST(sub, ps)
#define mulps				SINST(mul, ps)
#define divps				SINST(div, ps)
#define addss				SINST(add, ss)
#define subss				SINST(sub, ss)
#define mulss				SINST(mul, ss)
#define divss				SINST(div, ss)
#define maxps				SINST(max, ps)
#define maxss				SINST(max, ss)
#define minps				SINST(min, ps)
#define minss				SINST(min, ss)
#define rcpps				SINST(rcp,   ps)
#define rsqrtps				SINST(rsqrt, ps)
#define sqrtps				SINST(sqrt,  ps)
#define rcpss				SINST(rcp,   ss)
#define rsqrtss				SINST(rsqrt, ss)
#define sqrtss				SINST(sqrt,  ss)

// SSE integer ops
#define pmulhuw				INST(pmulhuw)
#define pavgb				INST(pavgb)
#define pavgw				INST(pavgw)
#define psadbw				INST(psadbw)
#define pmaxub				INST(pmaxub)
#define pmaxsw				INST(pmaxsw)
#define pminub				INST(pminub)
#define pminsw				INST(pminsw)
#define pextrw				INST(pextrw)
#define pinsrw				INST(pinsrw)
#define pmovmskb			INST(pmovmskb)
#define maskmovq			INST(maskmovq)
#define movntq				INST(movntq)
#define sfence				INST(sfence)
#define pshufw				INST(pshufw)


// SSE logical ops
#define andps				SINST(and,  ps)
#define andnps				SINST(andn, ps)
#define orps				SINST(or,   ps)
#define xorps				SINST(xor,  ps)


// SSE comparison ops
#define comieq				CINST(comieq)
#define comineq				CINST(comineq)
#define comilt				CINST(comilt)
#define comile				CINST(comile)
#define comigt				CINST(comigt)
#define comige				CINST(comige)
#define ucomieq				CINST(ucomieq)
#define ucomineq			CINST(ucomineq)
#define ucomilt				CINST(ucomilt)
#define ucomile				CINST(ucomile)
#define ucomigt				CINST(ucomigt)
#define ucomige				CINST(ucomige)
#define cmpeqps				SINST(cmpeq,    ps)
#define cmpltps				SINST(cmplt,    ps)
#define cmpleps				SINST(cmple,    ps)
#define cmpgtps				SINST(cmpgt,    ps)
#define cmpgeps				SINST(cmpge,    ps)
#define cmpunordps			SINST(cmpunord, ps)
#define cmpneqps			SINST(cmpneq,   ps)
#define cmpnltps			SINST(cmpnlt,   ps)
#define cmpnleps			SINST(cmpnle,   ps)
#define cmpngtps			SINST(cmpngt,   ps)
#define cmpngeps			SINST(cmpnge,   ps)
#define cmpordps			SINST(cmpord,   ps)
#define cmpeqss				SINST(cmpeq,    ss)
#define cmpltss				SINST(cmplt,    ss)
#define cmpless				SINST(cmple,    ss)
#define cmpunordss			SINST(cmpunord, ss)
#define cmpneqss			SINST(cmpneq,   ss)
#define cmpnltss			SINST(cmpnlt,   ss)
#define cmpnless			SINST(cmpnle,   ss)
#define cmpordss			SINST(cmpord,   ss)

// SSE conversion ops
#define unpckhps			SINST(UNPACKHI, ps)
#define unpcklps			SINST(UNPACKLO, ps)
#define cvtpi2ps			SINST(cvt,  pi2ps)
#define cvtsi2ss			SINST(cvt,  si2ss)
#define cvtps2pi			SINST(cvt,  ps2pi)
#define cvtss2si			SINST(cvt,  ss2si)
#define cvttps2pi			SINST(cvtt, ps2pi)
#define cvttss2si			SINST(cvtt, ss2si)

// SSE misc ops
#define movss				SINST(MOVE,   ss)
#define movhlps				SINST(MOVEHL, ps)
#define movlhps				SINST(MOVELH, ps)

#define shufps				_mm_shuffle_ps

#define movntps				SINST(movnt,   ps)
#define movmskps			SINST(MOVEMASK, ps)

#define loadss				SINST(load, ss)
#define setss				SINST(set,  ss)
#define setzerops			SINST(setzero, ps)


// SSE2 arithmetic ops
#define addpd				SINST(add, pd)
#define subpd				SINST(sub, pd)
#define mulpd				SINST(mul, pd)
#define divpd				SINST(div, pd)

#define addsd				SINST(add, sd)
#define subsd				SINST(sub, sd)
#define mulsd				SINST(mul, sd)
#define divsd				SINST(div, sd)

#define maxpd				SINST(max, pd)
#define maxsd				SINST(max, sd)
#define minpd				SINST(min, pd)
#define minsd				SINST(min, sd)
#define rcppd				SINST(rcp,   pd)
#define rsqrtpd				SINST(rsqrt, pd)
#define sqrtpd				SINST(sqrt,  pd)
#define rcpsd				SINST(rcp,   sd)
#define rsqrtsd				SINST(rsqrt, sd)
#define sqrtsd				SINST(sqrt,  sd)

// SSE2 logical ops
#define andpd				SINST(and,  pd)
#define andnpd				SINST(andn, pd)
#define orpd				SINST(or,   pd)
#define xorpd				SINST(xor,  pd)

// SSE2 comparison ops
#define comisdeq			CINST2(comi,   eq)
#define comisdneq			CINST2(comi,  neq)
#define comisdlt			CINST2(comi,   lt)
#define comisdle			CINST2(comi,   le)
#define comisdgt			CINST2(comi,   gt)
#define comisdge			CINST2(comi,   ge)
#define ucomisdeq			CINST2(ucomi,  eq)
#define ucomisdneq			CINST2(ucomin, eq)
#define ucomisdlt			CINST2(ucomi,  lt)
#define ucomisdle			CINST2(ucomi,  le)
#define ucomisdgt			CINST2(ucomi,  gt)
#define ucomisdge			CINST2(ucomi,  ge)
#define cmpeqpd				SINST(cmpeq,    pd)
#define cmpltpd				SINST(cmplt,    pd)
#define cmplepd				SINST(cmple,    pd)
#define cmpgtpd				SINST(cmpgt,    pd)
#define cmpgepd				SINST(cmpge,    pd)
#define cmpunordpd			SINST(cmpunord, pd)
#define cmpneqpd			SINST(cmpneq,   pd)
#define cmpnltpd			SINST(cmpnlt,   pd)
#define cmpnlepd			INST(cmpnle,   pd)
#define cmpngtpd			SINST(cmpngt,   pd)
#define cmpngepd			SINST(cmpnge,   pd)
#define cmpordpd			SINST(cmpord,   pd)
#define cmpeqsd    			SINST(cmpeq,    sd)
#define cmpltsd    			SINST(cmplt,    sd)
#define cmplesd    			SINST(cmple,    sd)
#define cmpunordsd 			SINST(cmpunord, sd)
#define cmpneqsd   			SINST(cmpneq,   sd)
#define cmpnltsd   			SINST(cmpnlt,   sd)
#define cmpnlesd   			SINST(cmpnle,   sd)
#define cmpordsd   			SINST(cmpord,   sd)

// SSE2 conversion ops
#define unpckhpd 			SINST(UNPACKHI, pd)
#define unpcklpd 			SINST(UNPACKLO, pd)

#define cvtpi2pd  			SINST(cvt,  pi2pd)
#define cvtpd2pi  			SINST(cvt,  pd2pi)
#define cvtpd2ps  			SINST(cvt,  pd2ps)

#define cvtdq2pd  			SINST(cvt,  dq2pd)
#define cvtdq2ps  			SINST(cvt,  dq2ps)
#define cvtpd2dq  			SINST(cvt,  pd2dq)
#define cvttpd2dq 			SINST(cvtt, pd2dq)

#define cvttpd2pi 			_mm_cvttpd_pi32
#define cvtsi2sd  			_mm_cvtsi32_sd
#define cvtsd2si  			_mm_cvtsd_si32

#define cvtps2dq  			SINST(cvt,  ps2dq)
#define cvttps2dq 			SINST(cvtt, ps2dq)
#define cvtps2pd  			SINST(cvt,  ps2pd)
#define cvttsd2si 			SINST(cvtt, sd2si)
#define cvtsd2ss  			SINST(cvt,  sd2ss)
#define cvtss2sd  			SINST(cvt,  ss2sd)


// SSE2 misc ops
#define movsd    			SINST(MOVE,   sd)
#define movhlpd  			SINST(MOVEHL, pd)
#define movlhpd  			SINST(MOVELH, pd)
#define shufpd   			SINST(SHUF,   pd)
#define movntpd  			SINST(movnt,   pd)
#define movmskpd 			SINST(MOVEMASK, pd)

#define loadsd 				SINST(load, sd)
#define setsd  				SINST(set,  sd)
#define setzeropd 			SINST(setzero, pd)

namespace MATH
{


	inline float dot(v4sf u, v4sf v)
	{
		v4sf m = mulps(u, v);
		v4sf f = shufps(m, m, SHUFFLE(2, 3, 0, 1));
		m = addps(m, f);
		f = shufps(m, m, SHUFFLE(1, 0, 3, 2));
		m = addss(m, f);
		return *((float *) &m);
	}

	inline v4sf dot4(v4sf u, v4sf v)
	{
		v4sf m = mulps(u, v);
		v4sf f = shufps(m, m, SHUFFLE(2, 3, 0, 1));
		m = addps(m, f);
		f = shufps(m, m, SHUFFLE(1, 0, 3, 2));
		return addps(m, f);
	}

	inline v4sf cross(v4sf u, v4sf v)
	{
		v4sf u_zxy = shufps(u, u, SHUFFLE(2, 0, 1, 3));
		v4sf u_yzx = shufps(u, u, SHUFFLE(1, 2, 0, 3));
		v4sf v_yzx = shufps(v, v, SHUFFLE(1, 2, 0, 3));
		v4sf v_zxy = shufps(v, v, SHUFFLE(2, 0, 1, 3));
		return subps(mulps(u_yzx, v_zxy), mulps(u_zxy, v_yzx));
	}




};//! end namespace MATH




#endif //! end __SIMD_INCLUDE__
