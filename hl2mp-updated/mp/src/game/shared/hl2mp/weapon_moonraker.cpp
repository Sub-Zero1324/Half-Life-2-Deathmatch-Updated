//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "AmmoDef.h"
#include "IEffects.h"
#include "particle_parse.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
	#include "c_te_effect_dispatch.h"
    #include "input.h"


#include "particles_simple.h"
#include "particles_localspace.h"
#include "dlight.h"
#include "iefx.h"
#include "clientsideeffects.h"
#include "ClientEffectPrecacheSystem.h"
#include "glow_overlay.h"
#include "effect_dispatch_data.h"
#include "c_te_effect_dispatch.h"
#include "tier0/vprof.h"
#include "tier1/keyvalues.h"
#include "effect_color_tables.h"
#include "iviewrender_beams.h"
#include "view.h"
#include "ieffects.h"
#include "fx.h"
#include "c_te_legacytempents.h"
#include "toolframework_client.h"


#else
	#include "hl2mp_player.h"
	#include "te_effect_dispatch.h"
#endif

#include "effect_dispatch_data.h"


#include "weapon_hl2mpbase_machinegun.h"

#define	MOONRAKER_FASTEST_REFIRE_TIME		0.1f
#define	MOONRAKER_FASTEST_DRY_REFIRE_TIME	0.2f

#ifdef CLIENT_DLL
#define CWeaponMoonRaker C_WeaponMoonRaker
#endif



//-----------------------------------------------------------------------------
// CWeaponMoonRaker
//-----------------------------------------------------------------------------

class CWeaponMoonRaker : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponMoonRaker, CHL2MPMachineGun );

	CWeaponMoonRaker(void);

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	Precache( void );
	void	ItemPostFrame( void );
	void	ItemPreFrame( void );
	void	ItemBusyFrame( void );
	void	PrimaryAttack( const Vector &vecTracerSrc, const trace_t &tr, int iTracerType );
	void    DrawBeam( void );
	void	DryFire( void );
    void	DoImpactEffect( trace_t &tr, int nDamageType );
	void	UpdatePenaltyTime( void );
	const char *GetTracerType( void ) { return "MoonTracer"; }

			virtual void MakeTracer( const Vector &vecTracerSrc, const trace_t &tr, int iTracerType );
#ifdef CLIENT_DLL
	virtual void ProcessMuzzleFlashEvent();
#else
	virtual int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	// NPC Functions
	virtual void	FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, bool bUseWeaponAngles );
#endif


#ifdef CLIENT_DLL
	//void	UpdateParticleSystem( void );
	virtual void			OnDataChanged( DataUpdateType_t updateType );
	CNewParticleEffect*m_hParticlesystem;
	CParticleProperty *pProp;
	bool m_bFlame_activate_local;
#endif

#ifndef CLIENT_DLL
	void	Fire( const Vector &vecOrigSrc, const Vector &vecDir );
	void	UpdateEffect( const Vector &startPoint, const Vector &endPoint );
	void	CreateEffect( void );
	void	DestroyEffect( void );
#endif


	virtual const Vector& GetBulletSpread( void )
	{		
		static Vector cone;

		return cone;
	}
	
	virtual int	GetMinBurst() 
	{ 
		return 1; 
	}

	virtual int	GetMaxBurst() 
	{ 
		return 3; 
	}

	virtual float GetFireRate( void ) 
	{
		return 0.5f; 
	}
	DECLARE_ACTTABLE();

private:
	CNetworkVar( float,	m_flSoonestPrimaryAttack );
	CNetworkVar( float,	m_flLastAttackTime );
	CNetworkVar( int,	m_nNumShotsFired );
	CNetworkVar( bool, m_bFlame_activate )

private:
	CWeaponMoonRaker( const CWeaponMoonRaker & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMoonRaker, DT_WeaponMoonRaker )

BEGIN_NETWORK_TABLE( CWeaponMoonRaker, DT_WeaponMoonRaker )
#ifdef CLIENT_DLL
	RecvPropTime( RECVINFO( m_flSoonestPrimaryAttack ) ),
	RecvPropTime( RECVINFO( m_flLastAttackTime ) ),
	RecvPropInt( RECVINFO( m_nNumShotsFired ) ),
	RecvPropBool( RECVINFO( m_bFlame_activate ) ),
#else
	SendPropTime( SENDINFO( m_flSoonestPrimaryAttack ) ),
	SendPropTime( SENDINFO( m_flLastAttackTime ) ),
	SendPropInt( SENDINFO( m_nNumShotsFired ) ),
    SendPropBool( SENDINFO( m_bFlame_activate ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponMoonRaker )
	DEFINE_PRED_FIELD( m_flSoonestPrimaryAttack, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flLastAttackTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nNumShotsFired, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_moonraker, CWeaponMoonRaker );
PRECACHE_WEAPON_REGISTER( weapon_moonraker );

acttable_t CWeaponMoonRaker::m_acttable[] = 
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PISTOL,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_PISTOL,					false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_PISTOL,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_PISTOL,					false },
};

IMPLEMENT_ACTTABLE( CWeaponMoonRaker );
ConVar sk_plr_max_dmg_moonraker( "sk_plr_max_dmg_moonraker", "2" );
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponMoonRaker::CWeaponMoonRaker( void )
{
	m_flSoonestPrimaryAttack = gpGlobals->curtime;

	m_fMinRange1		= 24;
	m_fMaxRange1		= 1500;
	m_fMinRange2		= 24;
	m_fMaxRange2		= 200;

	m_bFiresUnderwater	= true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::Precache( void )
{

	PrecacheParticleSystem( "muzzle_moonraker_flash" );
	PrecacheParticleSystem( "tracer_laser" );

	BaseClass::Precache();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
#ifndef CLIENT_DLL
void CWeaponMoonRaker::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
	{
		return;
	}

	Vector vecDest	= vecOrigSrc + (vecDir * 300);
	m_bFlame_activate = true;
	trace_t	tr;
	UTIL_TraceHull( vecOrigSrc, vecDest, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr );

	if ( tr.allsolid )
		return;

	CBaseEntity *pEntity = tr.m_pEnt;
	if ( pEntity == NULL )
		return;



	
	Vector vecUp, vecRight;
	QAngle angDir;

	VectorAngles( vecDir, angDir );
	AngleVectors( angDir, NULL, &vecRight, &vecUp );

	Vector tmpSrc = vecOrigSrc + (vecUp * -8) + (vecRight * 3);
	UpdateEffect( tmpSrc, tr.endpos );
}
#endif
#ifndef CLIENT_DLL
void CWeaponMoonRaker::UpdateEffect( const Vector &startPoint, const Vector &endPoint )
{
	m_bFlame_activate = true;
}
#endif
#ifndef CLIENT_DLL
void CWeaponMoonRaker::CreateEffect( void )
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
void CWeaponMoonRaker::DestroyEffect( void )
{
	m_bFlame_activate = false;
}
#endif


/*
#ifdef CLIENT_DLL
void CWeaponMoonRaker::UpdateParticleSystem( void )
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
        m_hParticlesystem = pProp->Create( "muzzle_moonraker_flash", PATTACH_POINT_FOLLOW, "muzzle" ); //loadup_trails
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
#endif*/
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::DryFire( void )
{
	WeaponSound( EMPTY );
	SendWeaponAnim( ACT_VM_DRYFIRE );
	
	m_flSoonestPrimaryAttack	= gpGlobals->curtime + MOONRAKER_FASTEST_DRY_REFIRE_TIME;
	m_flNextPrimaryAttack		= gpGlobals->curtime + SequenceDuration();
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::DrawBeam( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if(pOwner == NULL)
		return;


	//Shoot a shot straight
	Vector  startPos= pOwner->Weapon_ShootPosition();
	Vector  aimDir  = pOwner->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector  endPos  = startPos + ( aimDir * MAX_TRACE_LENGTH );



#ifdef CLIENT_DLL
			Vector origin;
			QAngle angles;
			CBaseViewModel *pViewModel = pOwner->GetViewModel();
			pViewModel->GetAttachment(LookupAttachment("muzzle"),origin,angles);
			//g_pEffects->EnergySplash( origin, -origin, false );


//UTIL_Tracer( origin, -origin, 0, MAX_TRACE_LENGTH, 5000, false, "HunterTracer" );
#endif


}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::PrimaryAttack( const Vector &vecTracerSrc, const trace_t &tr, int iTracerType )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( !pPlayer )
	{
		return;
	}


		if ( pPlayer == NULL )
	{
		BaseClass::MakeTracer( vecTracerSrc, tr, iTracerType );
		return;
	}

	if ( ( gpGlobals->curtime - m_flLastAttackTime ) > 0.5f )
	{
		m_nNumShotsFired = 0;
	}
	else
	{
		m_nNumShotsFired++;
	}

	m_flLastAttackTime = gpGlobals->curtime;
	m_flSoonestPrimaryAttack = gpGlobals->curtime + MOONRAKER_FASTEST_REFIRE_TIME;



//DrawBeam();
	Vector vecAiming	= pPlayer->GetAutoaimVector( 0 );
	Vector vecSrc		= pPlayer->Weapon_ShootPosition( );

#ifndef CLIENT_DLL
			Fire( vecSrc, vecAiming );
#endif	
#ifdef CLIENT_DLL
//UpdateParticleSystem();
#endif
	BaseClass::PrimaryAttack();

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::UpdatePenaltyTime( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Check our penalty time decay
	if ( ( ( pOwner->m_nButtons & IN_ATTACK ) == false ) && ( m_flSoonestPrimaryAttack < gpGlobals->curtime ) )
	{

	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::ItemPreFrame( void )
{
	UpdatePenaltyTime();

	BaseClass::ItemPreFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::ItemBusyFrame( void )
{
	UpdatePenaltyTime();

	BaseClass::ItemBusyFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Allows firing as fast as button is pressed
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();
#ifdef CLIENT_DLL
//UpdateParticleSystem();
#endif

	
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;
	
	if ( pOwner->m_nButtons & IN_ATTACK2 )
	{
		m_flLastAttackTime = gpGlobals->curtime + MOONRAKER_FASTEST_REFIRE_TIME;
		m_flSoonestPrimaryAttack = gpGlobals->curtime + MOONRAKER_FASTEST_REFIRE_TIME;
		m_flNextPrimaryAttack = gpGlobals->curtime + MOONRAKER_FASTEST_REFIRE_TIME;
	}

	//Allow a refire as fast as the player can click
	if ( ( ( pOwner->m_nButtons & IN_ATTACK ) == false ) && ( m_flSoonestPrimaryAttack < gpGlobals->curtime ) )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime - 0.1f;
	}
	else if ( ( pOwner->m_nButtons & IN_ATTACK ) && ( m_flNextPrimaryAttack < gpGlobals->curtime ) && ( m_iClip1 <= 0 ) )
	{
		DryFire();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &tr - 
//			nDamageType - 
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::DoImpactEffect( trace_t &tr, int nDamageType )
{
	CEffectData data;

	data.m_vOrigin = tr.endpos + ( tr.plane.normal * 1.0f );
	data.m_vNormal = tr.plane.normal;

	DispatchEffect( "AR2Impact", data );

	BaseClass::DoImpactEffect( tr, nDamageType );
}
//-----------------------------------------------------------------------------
// Purpose: Starts the client-side version thinking
//-----------------------------------------------------------------------------
#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: Starts the client-side version thinking
//-----------------------------------------------------------------------------
void CWeaponMoonRaker::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	if (m_bFlame_activate != m_bFlame_activate_local)
	{
	//	UpdateParticleSystem();
	}
}
#endif


void CWeaponMoonRaker::MakeTracer( const Vector &vecTracerSrc, const trace_t &tr, int iTracerType )
{
	CBaseEntity *pOwner = GetOwner();

	if ( pOwner == NULL )
	{
		BaseClass::MakeTracer( vecTracerSrc, tr, iTracerType );
		return;
	}

	Vector vNewSrc = vecTracerSrc;
	int iEntIndex = pOwner->entindex();

	if ( g_pGameRules->IsMultiplayer() )
	{
		iEntIndex = entindex();
	}

	int iAttachment = GetTracerAttachment();
	UTIL_ParticleTracer( "muzzle_moonraker_flash", vNewSrc, tr.endpos, iEntIndex, iAttachment, false );
	UTIL_ParticleTracer( "tracer_laser", vNewSrc, tr.endpos, iEntIndex, iAttachment, false );
}

#ifdef CLIENT_DLL

extern ConVar muzzleflash_light;
extern void FormatViewModelAttachment( Vector &vOrigin, bool bInverse );
void CWeaponMoonRaker::ProcessMuzzleFlashEvent()
{
	// If we have an attachment, then stick a light on it.
	if ( muzzleflash_light.GetBool() )
	{
		Vector vAttachment;
		QAngle dummyAngles;

		if ( !GetAttachment( "muzzle", vAttachment, dummyAngles ) )
			vAttachment = GetAbsOrigin();

		// Format the position for first person view
		if ( GetOwner() == CBasePlayer::GetLocalPlayer() )
			::FormatViewModelAttachment( vAttachment, true );
	
		// Make an elight (entities)
		dlight_t *el = effects->CL_AllocElight( LIGHT_INDEX_MUZZLEFLASH + index );
		el->origin = vAttachment;
		el->radius = random->RandomInt( 64, 80 ); 
		el->decay = el->radius / 0.05f;
		el->die = gpGlobals->curtime + 0.05f;
		el->color.r = 83;
		el->color.g = 169;
		el->color.b = 255;
		el->color.exponent = 5;

		// Make a dlight (world)
		dlight_t *dl = effects->CL_AllocDlight( LIGHT_INDEX_MUZZLEFLASH + index );
		dl->origin = vAttachment;
		dl->radius = random->RandomInt( 64, 80 ); 
		dl->decay = el->radius / 0.05f;
		dl->flags |= DLIGHT_NO_MODEL_ILLUMINATION;
		dl->die = gpGlobals->curtime + 0.05f;
		dl->color.r = 83;
		dl->color.g = 169;
		dl->color.b = 255;
		dl->color.exponent = 5;
	}
}

#endif


#ifdef GAME_DLL
void CWeaponMoonRaker::FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, bool bUseWeaponAngles )
{
//	BaseClass::FireNPCPrimaryAttack( pOperator, bUseWeaponAngles );
//	GetOwner()->SetAmmoCount( AMMO_MOONRAKER_MAX, m_iPrimaryAmmoType );
}
#endif