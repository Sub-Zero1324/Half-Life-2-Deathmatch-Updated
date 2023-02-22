//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef GRENADE_HOPWIRE_H
#define GRENADE_HOPWIRE_H
#ifdef _WIN32
#pragma once
#endif

#include "basegrenade_shared.h"
#include "sprite.h"

extern ConVar hopwire_trap;

//class CGravityVortexController;

class CGrenadeHopwire : public CBaseGrenade
{
	DECLARE_CLASS( CGrenadeHopwire, CBaseGrenade );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

public:
	void	Spawn( void );
	//CBaseCombatCharacter	*GetOwner() const;
	void	Precache( void );
	bool	CreateVPhysics( void );
	void	SetTimer( float timer );
	void	SetVelocity( const Vector &velocity, const AngularImpulse &angVelocity );
	void	Detonate( void );
	
	void	EndThink( void );		// Last think before going away
	void	CombatThink( void );	// Makes the main explosion go off


	CGrenadeHopwire(void) : m_flEndTime(0.0f), m_flRadius(256), m_flStrength(256), m_flMass(0.0f) {}
	float	GetConsumedMass(void) const;

	static CGrenadeHopwire *Create(const Vector &origin, float radius, float strength, float duration);
protected:

	void	KillStriders( void );

	CHandle<CGrenadeHopwire>	m_hVortexController;

private:

	void	ConsumeEntity(CBaseEntity *pEnt);
	void	PullPlayersInRange(void);
	bool	KillNPCInRange(CBaseEntity *pVictim, IPhysicsObject **pPhysObj);
	void	CreateDenseBall(void);
	void	PullThink(void);
	void	StartPull(const Vector &origin, float radius, float strength, float duration);

	float	m_flMass;		// Mass consumed by the vortex
	float	m_flEndTime;	// Time when the vortex will stop functioning
	float	m_flRadius;		// Area of effect for the vortex
	float	m_flStrength;	// Pulling strength of the vortex
	//EHANDLE		m_hOwner;
};

extern CBaseGrenade *HopWire_Create( const Vector &position, const QAngle &angles, const Vector &velocity, const AngularImpulse &angVelocity, CBaseEntity *pOwner, float timer );

#endif // GRENADE_HOPWIRE_H
