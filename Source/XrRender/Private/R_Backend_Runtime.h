#ifndef R_BACKEND_RUNTIMEH
#define R_BACKEND_RUNTIMEH
#pragma once

#include "sh_texture.h"
#include "sh_constant.h"
#include "sh_rt.h"

IC	void	CBackend::set_xform_world	(const Fmatrix& M)
{ 
	xforms.set_W(M);	
}
IC	void	CBackend::set_xform_view	(const Fmatrix& M)					
{ 
	xforms.set_V(M);	
}
IC	void	CBackend::set_xform_project	(const Fmatrix& M)
{ 
	xforms.set_P(M);	
}
IC	const Fmatrix&	CBackend::get_xform_world	()	{ return xforms.get_W();	}
IC	const Fmatrix&	CBackend::get_xform_view	()	{ return xforms.get_V();	}
IC	const Fmatrix&	CBackend::get_xform_project	()	{ return xforms.get_P();	}

IC	ID3DRenderTargetView* CBackend::get_RT(u32 ID)
{
	VERIFY((ID>=0)&&(ID<4));

	return pRT[ID];
}

IC	ID3DDepthStencilView* CBackend::get_ZB				()
{
	return pZB;
}

ICF void	CBackend::set_States		(ID3DState* _state)
{
//	DX10 Manages states using it's own algorithm. Don't mess with it.
#if !defined(USE_DX10) && !defined(USE_DX11)
	if (state!=_state)
#endif	//	USE_DX10
	{
		PGO				(Msg("PGO:state_block"));
#ifdef DEBUG
		stat.states		++;
#endif
		state			= _state;
		state->Apply	();
	}
}

#ifdef _EDITOR
IC void CBackend::set_Matrices			(SMatrixList*	_M)
{
	if (M != _M)
	{
		M = _M;
		if (M)	{
			for (u32 it=0; it<M->size(); it++)
			{
				CMatrix*	mat = &*((*M)[it]);
				if (mat && matrices[it]!=mat)
				{
					matrices	[it]	= mat;
					mat->Calculate		();
					set_xform			(D3DTS_TEXTURE0+it,mat->xform);
	//				stat.matrices		++;
				}
			}
		}
	}
}
#endif

IC void CBackend::set_Element			(ShaderElement* S, u32	pass)
{
	SPass&	P		= *(S->passes[pass]);
	set_States		(P.state);
	set_PS			(P.ps);
#if defined(USE_DX10) || defined(USE_DX11)
	set_GS			(P.gs);
#ifdef USE_DX11
	set_HS			(P.hs);
	set_DS			(P.ds);
	set_CS			(P.cs);
#endif
#endif	//	USE_DX10
	set_Constants	(P.constants);
#ifdef _EDITOR
	set_Matrices	(P.M);
#endif
}

ICF void CBackend::set_Shader			(Shader* S, u32 pass)
{
	set_Element			(S->E[0],pass);
}

#endif
