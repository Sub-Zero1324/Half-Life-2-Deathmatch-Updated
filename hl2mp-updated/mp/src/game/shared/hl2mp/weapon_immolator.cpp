//========= Copyright © 1996-2010, Valve Corporation and .kave, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "sprite.h"
#include "beam_shared.h"
#include "beam_flags.h"
#include "gamerules.h"
#include "particle_parse.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
    #include "ClientEffectPrecacheSystem.h"
    #include "particles_new.h"
    #include "particles_simple.h"
    #include "particles_attractor.h"
    #include "input.h"
#else
	#include "hl2mp_player.h"
    #include "soundent.h"
    #include "AI_BaseNPC.h"
    #include "player.h"
    #include "basecombatcharacter.h"
    #include "physics_prop_ragdoll.h"
    #include "props.h"
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#ifdef CLIENT_DLL
#define CWeaponImmolator C_WeaponImmolator
#endif

#define IMMOLATOR_PULSE_INTERVAL			0.1
#define IMMOLATOR_DISCHARGE_INTERVAL		0.1

ConVar sk_plr_dmg_immolator_wide			( "sk_plr_dmg_immolator_wide", "4", FCVAR_REPLICATED );


enum IMMOLATOR_FIRESTATE { FIRE_OFF, FIRE_STARTUP, FIRE_CHARGE };

//-----------------------------------------------------------------------------
// CWeaponImmolator
//-----------------------------------------------------------------------------

class CWeaponImmolator : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeaponImmolator, CBaseHL2MPCombatWeapon );
public:

	CWeaponImmolator( void );
#ifdef CLIENT_DLL
	void	UpdateParticleSystem( void );
	virtual void			OnDataChanged( DataUpdateType_t updateType );
#endif
	void	PrimaryAttack( void );
	void	WeaponIdle( void );
	bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	void	Precache( void );
	bool	Deploy( void );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

private:
	bool	HasAmmo( void );
	void	UseAmmo( int count );
	void	Attack( void );
	void	EndAttack( void );
#ifndef CLIENT_DLL
	void	Fire( const Vector &vecOrigSrc, const Vector &vecDir );
	void	UpdateEffect( const Vector &startPoint, const Vector &endPoint );
	void	CreateEffect( void );
	void	DestroyEffect( void );
#endif
#ifdef CLIENT_DLL
	CNewParticleEffect*m_hParticlesystem;
	CParticleProperty *pProp;
	bool m_bFlame_activate_local;
#endif
	CNetworkVar( bool, m_bFlame_activate )
	IMMOLATOR_FIRESTATE		m_fireState;
	float				m_flAmmoUseTime;	// since we use < 1 point of ammo per update, we subtract ammo on a timer.
	//float				m_flShakeTime;
	float				m_flStartFireTime;
	float				m_flDmgTime;
	CWeaponImmolator( const CWeaponImmolator & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponImmolator, DT_WeaponImmolator )

BEGIN_NETWORK_TABLE( CWeaponImmolator, DT_WeaponImmolator )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bFlame_activate ) ),
#else
	SendPropBool( SENDINFO( m_bFlame_activate ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponImmolator )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_immolator, CWeaponImmolator );
PRECACHE_WEAPON_REGISTER( weapon_immolator );

#ifndef CLIENT_DLL
acttable_t CWeaponImmolator::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_FLAMETHROWER,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_FLAMETHROWER,			    false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_FLAMETHROWER,					    false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_FLAMETHROWER,			    false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_FLAMETHROWER,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_FLAMETHROWER,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_FLAMETHROWER,		    false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_FLAMETHROWER,		    false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_FLAMETHROWER,					false },
};

IMPLEMENT_ACTTABLE( CWeaponImmolator );
#endif
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponImmolator::CWeaponImmolator( void )
{
	m_bReloadsSingly	= false;
	m_bFiresUnderwater	= true;
	m_bFlame_activate   = false;
#ifdef CLIENT_DLL
	m_hParticlesystem = NULL;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponImmolator::Precache( void )
{
	PrecacheParticleSystem( "flamethrower_underwater" );
	PrecacheParticleSystem( "flamethrower" );
	PrecacheParticleSystem( "Weapon_Combine_Ion_Cannon" );

PrecacheScriptSound( "Weapon_FlameThrower.FireLoop" );
PrecacheScriptSound( "Weapon_FlameThrower.FireStart" );

	BaseClass::Precache();
}

bool CWeaponImmolator::Deploy( void )
{
	m_fireState = FIRE_OFF;
	m_bFlame_activate   = false;
	return BaseClass::Deploy();
}

bool CWeaponImmolator::HasAmmo( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
	{
		return false;
	}

	if ( pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
		return false;

	return true;
}

void CWeaponImmolator::UseAmmo( int count )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
	{
		return;
	}

	if ( pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) >= count )
		pPlayer->RemoveAmmo( count, m_iPrimaryAmmoType );
	else
		pPlayer->RemoveAmmo( pPlayer->GetAmmoCount( m_iPrimaryAmmoType ), m_iPrimaryAmmoType );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponImmolator::PrimaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( !pPlayer )
	{
		return;
	}

			if ( gpGlobals->curtime >= m_flAmmoUseTime )
			{
				UseAmmo( 1 );
				m_flAmmoUseTime = gpGlobals->curtime + 0.2;
			}

	// don't fire underwater
	if ( pPlayer->GetWaterLevel() == 3 )
	{
		if ( m_fireState != FIRE_OFF )
		{
			EndAttack();
		}
		else
		{

		}

		m_flNextPrimaryAttack = gpGlobals->curtime + 0.5;
		return;
	}

	Vector vecAiming	= pPlayer->GetAutoaimVector( 0 );
	Vector vecSrc		= pPlayer->Weapon_ShootPosition( );

	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if ( !HasAmmo() )
			{
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.25;
				WeaponSound( EMPTY );
				return;
			}

			m_flAmmoUseTime = gpGlobals->curtime;// start using ammo ASAP.

			EmitSound( "Weapon_FlameThrower.FireStart" );

			SendWeaponAnim( ACT_VM_PRIMARYATTACK );
						
			//m_flShakeTime = 0;
			m_flStartFireTime = gpGlobals->curtime;

			SetWeaponIdleTime( gpGlobals->curtime + 0.1 );

			m_flDmgTime = gpGlobals->curtime + IMMOLATOR_PULSE_INTERVAL;
			m_fireState = FIRE_STARTUP;
		}
		break;

		case FIRE_STARTUP:
		{
#ifndef CLIENT_DLL
			Fire( vecSrc, vecAiming );
#endif		
			if ( gpGlobals->curtime >= ( m_flStartFireTime + 2.0 ) )
			{
				EmitSound( "Weapon_FlameThrower.FireLoop" );

				m_fireState = FIRE_CHARGE;
			}

			if ( !HasAmmo() )
			{
				EndAttack();
				m_flNextPrimaryAttack = gpGlobals->curtime + 1.0;
			}
		}
		case FIRE_CHARGE:
		{
#ifndef CLIENT_DLL
			Fire( vecSrc, vecAiming );
#endif		
			if ( !HasAmmo() )
			{
				EndAttack();
				m_flNextPrimaryAttack = gpGlobals->curtime + 1.0;
			}
		}
		break;
	}
}
#ifndef CLIENT_DLL
void CWeaponImmolator::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
	{
		return;
	}
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 450, 0.1 );
	Vector vecDest	= vecOrigSrc + (vecDir * 300);
	m_bFlame_activate = true;
	trace_t	tr;
	UTIL_TraceHull( vecOrigSrc, vecDest, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr );

	if ( tr.allsolid )
		return;

	CBaseEntity *pEntity = tr.m_pEnt;
	if ( pEntity == NULL )
		return;

	if ( m_flDmgTime < gpGlobals->curtime )
	{

if ( pPlayer->GetWaterLevel() == 3 )
{
		// Do not take damage under water
		if ( pEntity->m_takedamage != DAMAGE_NO )
		{

		}

}

else
{

		// wide mode does damage to the ent, and radius damage
		if ( pEntity->m_takedamage != DAMAGE_NO )
		{
			ClearMultiDamage();
			CTakeDamageInfo info( this, pPlayer, sk_plr_dmg_immolator_wide.GetFloat(), DMG_BURN | DMG_BULLET );
			CalculateMeleeDamageForce( &info, vecDir, tr.endpos );
			pEntity->DispatchTraceAttack( info, vecDir, &tr );
			ApplyMultiDamage();
		}

		if ( g_pGameRules->IsMultiplayer() )
		{
			// radius damage a little more potent in multiplayer.
			RadiusDamage( CTakeDamageInfo( this, pPlayer, sk_plr_dmg_immolator_wide.GetFloat() / 4, DMG_BURN | DMG_BULLET ), tr.endpos, 128, CLASS_PLAYER_ALLY, pPlayer );
		}
}
		if ( !pPlayer->IsAlive() )
			return;

		if ( g_pGameRules->IsMultiplayer() )
		{
			//multiplayer uses 5 ammo/second
			if ( gpGlobals->curtime >= m_flAmmoUseTime )
			{
				UseAmmo( 1 );
				m_flAmmoUseTime = gpGlobals->curtime + 0.2;
			}
		}
		else
		{
			// Wide mode uses 10 charges per second in single player
			if ( gpGlobals->curtime >= m_flAmmoUseTime )
			{
				UseAmmo( 1 );
				m_flAmmoUseTime = gpGlobals->curtime + 0.1;
			}
		}

		m_flDmgTime = gpGlobals->curtime + IMMOLATOR_DISCHARGE_INTERVAL;
	}
	Vector vecUp, vecRight;
	QAngle angDir;

	VectorAngles( vecDir, angDir );
	AngleVectors( angDir, NULL, &vecRight, &vecUp );

	Vector tmpSrc = vecOrigSrc + (vecUp * -8) + (vecRight * 3);
	UpdateEffect( tmpSrc, tr.endpos );
}
#endif
#ifndef CLIENT_DLL
void CWeaponImmolator::UpdateEffect( const Vector &startPoint, const Vector &endPoint )
{
	m_bFlame_activate = true;
}
#endif
#ifndef CLIENT_DLL
void CWeaponImmolator::CreateEffect( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
	{
		return;
	}

	DestroyEffect();
	m_bFlame_activate = true;

}
#endif
#ifndef CLIENT_DLL
void CWeaponImmolator::DestroyEffect( void )
{
	m_bFlame_activate = false;
}
#endif
void CWeaponImmolator::EndAttack( void )
{
	StopSound( "Weapon_FlameThrower.FireLoop" );
	StopSound( "Weapon_FlameThrower.FireStart" );
	
	if ( m_fireState != FIRE_OFF )
	{
		 EmitSound( "Weapon_Gluon.Off" );
	}

	SetWeaponIdleTime( gpGlobals->curtime + 2.0 );
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5;

	m_fireState = FIRE_OFF;
#ifndef CLIENT_DLL
	DestroyEffect();
#endif
}

bool CWeaponImmolator::Holster( CBaseCombatWeapon *pSwitchingTo )
{
    EndAttack();
	StopSound( "Weapon_FlameThrower.FireLoop" );
	StopSound( "Weapon_FlameThrower.FireStart" );

	return BaseClass::Holster( pSwitchingTo );
}

void CWeaponImmolator::WeaponIdle( void )
{
	if ( !HasWeaponIdleTimeElapsed() )
		return;

	if ( m_fireState != FIRE_OFF )
		 EndAttack();
	
	int iAnim;

	float flRand = random->RandomFloat( 0,1 );
	if ( flRand <= 0.5 )
	{
		iAnim = ACT_VM_IDLE;
	}
	else 
	{
		iAnim = ACT_VM_FIDGET;
	}

	SendWeaponAnim( iAnim );
}

#ifdef CLIENT_DLL
void CWeaponImmolator::UpdateParticleSystem( void )
{

  pProp = ParticleProp();
  if (m_hParticlesystem)
  {
    pProp->StopEmission(m_hParticlesystem);
    m_hParticlesystem = NULL;
  }
  if (m_bFlame_activate)
  {
	  if (m_hParticlesystem)
        {
          pProp->StopEmission(m_hParticlesystem);
          m_hParticlesystem = NULL;
        }


        CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
        Vector vecOrigin_attach;
if ( pPlayer->GetWaterLevel() == 3 )
{
        m_hParticlesystem = pProp->Create( "flamethrower_underwater", PATTACH_POINT_FOLLOW, "muzzle" ); //loadup_trails
}
else
m_hParticlesystem = pProp->Create( "flamethrower", PATTACH_POINT_FOLLOW, "muzzle" ); //loadup_trails

		//Tony; third person attachment
	    if ( pPlayer->IsLocalPlayer() && !::input->CAM_IsThirdPerson())
	    {
		   if ( m_hParticlesystem )
           {
		   pProp->AddControlPoint( m_hParticlesystem, 0, pPlayer->GetViewModel(), PATTACH_POINT_FOLLOW, "muzzle" );
		   }
		}
		else
		{
		}
  }
  else
  {
	  if (m_hParticlesystem)
        {
          pProp->StopEmission(m_hParticlesystem);
          m_hParticlesystem = NULL;
        }
  }

  m_bFlame_activate_local = m_bFlame_activate;
}
#endif
#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: Starts the client-side version thinking
//-----------------------------------------------------------------------------
void CWeaponImmolator::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	if (m_bFlame_activate != m_bFlame_activate_local)
	{
		UpdateParticleSystem();
	}
}
#endif
