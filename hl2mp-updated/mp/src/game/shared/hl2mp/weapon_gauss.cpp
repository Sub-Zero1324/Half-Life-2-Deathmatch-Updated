//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=Copyright 2015 SIOSPHERE/Sub-Zero-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//	PURPOSE: GAUSS GUN
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "cbase.h"
#include "beam_shared.h"
#include "AmmoDef.h"
#include "in_buttons.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "soundenvelope.h"
#include "particle_parse.h"
#include "IEffects.h"
#include "dlight.h"
#include "r_efx.h"
#ifdef CLIENT_DLL
#include "c_hl2mp_player.h"
#include "ClientEffectPrecacheSystem.h"
#include "c_te_effect_dispatch.h"
#include "particles_simple.h"
#include "particles_localspace.h"


#include "dlight.h"
#include "iefx.h"
#include "clientsideeffects.h"
#include "glow_overlay.h"
#include "effect_dispatch_data.h"
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
#ifdef CLIENT_DLL
#define CWeaponGauss C_WeaponGauss
#endif

//Gauss Definitions

#define GAUSS_BEAM_SPRITE "sprites/laserbeam.vmt"
#define GAUSS_BEAM_SPRITE2 "sprites/laserbeam.vmt"
#define	GAUSS_CHARGE_TIME			0.3f
#define	MAX_GAUSS_CHARGE			16
#define	MAX_GAUSS_CHARGE_TIME		3.8
#define	DANGER_GAUSS_CHARGE_TIME	10
#define GAUSS_GUN_SPIN				"gun_spin"
#define	GAUSS_GUN_SPIN_RATE			10

#define GAUSS_GUN_SPIN_FIRE1		"gun_spin"
#define	GAUSS_GUN_SPIN_RATE_FIRE1	10

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern Vector PointOnLineNearestPoint(const Vector& vStartPos, const Vector& vEndPos, const Vector& vPoint);

#ifdef CLIENT_DLL
CLIENTEFFECT_REGISTER_BEGIN(PrecacheEffectGauss)
CLIENTEFFECT_MATERIAL("sprites/laserbeam")
//CLIENTEFFECT_MATERIAL( "sprites/physbeam" )
CLIENTEFFECT_REGISTER_END()
#endif




class CWeaponGauss : public CBaseHL2MPCombatWeapon
{
public:

	DECLARE_CLASS(CWeaponGauss, CBaseHL2MPCombatWeapon);
	CWeaponGauss(void);

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	void	Spawn(void);
	void	Precache(void);
	void	PrimaryAttack(void);
	void	SecondaryAttack(void);
	void	AddViewKick(void);

	bool	Holster(CBaseCombatWeapon* pSwitchingTo = NULL);

	void	ItemPostFrame(void);

	float	GetFireRate(void) { return 0.2f; }
	void    MakeTracer(void);
	void	DoWallBreak(Vector startPos, Vector endPos, Vector aimDir, trace_t* ptr, CBasePlayer* pOwner, bool m_bBreakAll);
	bool	DidPunchThrough(trace_t* tr);


#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

protected:
	void	Fire(void);
	void	ChargedFire(void);
	void	ChargedFireFirstBeam(void);
	void	StopChargeSound(void);
	void	DrawBeam(const Vector& startPos, const Vector& endPos, float width, bool useMuzzle = false);
	void	DrawBeam2(const Vector& startPos, const Vector& endPos, float width, bool useMuzzle = false);
	void	IncreaseCharge(void);

	void	MuzzleLight(void);

	//int				m_nSpinPos;
private:
	CSoundPatch* m_sndCharge;

	CNetworkVar(bool, m_bCharging);
	CNetworkVar(bool, m_bChargeIndicated);

	CNetworkVar(float, m_flNextChargeTime);
	CNetworkVar(float, m_flChargeStartTime);
	CNetworkVar(float, m_flCoilMaxVelocity);
	CNetworkVar(float, m_flCoilVelocity);
	CNetworkVar(float, m_flCoilAngle);

	CNetworkVar(int, m_nSpinPos);
	CNetworkVar(int, m_nSpinPosFire1);


	CNetworkHandle(CBeam, m_pBeam);

};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponGauss, DT_WeaponGauss)

BEGIN_NETWORK_TABLE(CWeaponGauss, DT_WeaponGauss)
#ifdef CLIENT_DLL
RecvPropBool(RECVINFO(m_bCharging)),
RecvPropBool(RECVINFO(m_bChargeIndicated)),
RecvPropFloat(RECVINFO(m_flNextChargeTime)),
RecvPropFloat(RECVINFO(m_flChargeStartTime)),
RecvPropFloat(RECVINFO(m_flCoilMaxVelocity)),
RecvPropFloat(RECVINFO(m_flCoilVelocity)),
RecvPropFloat(RECVINFO(m_flCoilAngle)),
RecvPropEHandle(RECVINFO(m_pBeam)),
RecvPropInt(RECVINFO(m_nSpinPos)),
RecvPropInt(RECVINFO(m_nSpinPosFire1)),
#else
SendPropBool(SENDINFO(m_bCharging)),
SendPropBool(SENDINFO(m_bChargeIndicated)),
SendPropFloat(SENDINFO(m_flNextChargeTime)),
SendPropFloat(SENDINFO(m_flChargeStartTime)),
SendPropFloat(SENDINFO(m_flCoilMaxVelocity)),
SendPropFloat(SENDINFO(m_flCoilVelocity)),
SendPropFloat(SENDINFO(m_flCoilAngle)),
SendPropEHandle(SENDINFO(m_pBeam)),
SendPropInt(SENDINFO(m_nSpinPos)),
SendPropInt(SENDINFO(m_nSpinPosFire1)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeaponGauss)
DEFINE_PRED_FIELD(m_pBeam, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE),
DEFINE_FIELD(m_nSpinPos, FIELD_INTEGER),
DEFINE_FIELD(m_nSpinPosFire1, FIELD_INTEGER),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS(weapon_gauss, CWeaponGauss);
PRECACHE_WEAPON_REGISTER(weapon_gauss);

#ifndef CLIENT_DLL

acttable_t	CWeaponGauss::m_acttable[] =
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_PHYSGUN,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_PHYSGUN,					false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_PHYSGUN,			false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_PHYSGUN,			false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_PHYSGUN,		false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_PHYSGUN,					false },
};

IMPLEMENT_ACTTABLE(CWeaponGauss);

#endif

ConVar sk_dmg_gauss("sk_dmg_gauss", "20");
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponGauss::CWeaponGauss(void)
{
	m_flNextChargeTime = 0;
	m_flChargeStartTime = 0;
	m_sndCharge = NULL;
	m_bCharging = false;
	m_bChargeIndicated = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::Precache(void)
{
	PrecacheModel(GAUSS_BEAM_SPRITE);
	PrecacheModel(GAUSS_BEAM_SPRITE2);
	PrecacheParticleSystem("tau_tracer");
	PrecacheParticleSystem("hl2mmod_muzzleflash_gauss");
	PrecacheParticleSystem("hl2mmod_muzzleflash_gauss_alt");
	//PrecacheParticleSystem("hl2mmod_muzzleflash_gauss_charge");

	PrecacheParticleSystem("hl2mmod_tracer_gaussrifle_tracer_buggy1");
	PrecacheParticleSystem("hl2mmod_tracer_gaussrifle_tracer_buggy2");
#ifndef CLIENT_DLL
	enginesound->PrecacheSound("weapons/gauss/chargeloop.wav");
	PrecacheScriptSound("Weapon_Gauss.Special1");
#endif
	BaseClass::Precache();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::Spawn(void)
{

	SetActivity(ACT_HL2MP_IDLE);
	m_nSpinPos = 0;
	SetPoseParameter(GAUSS_GUN_SPIN, m_nSpinPos);

	m_nSpinPosFire1 = 0;
	SetPoseParameter(GAUSS_GUN_SPIN_FIRE1, m_nSpinPosFire1);

	BaseClass::Spawn();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::PrimaryAttack(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());
	if (pOwner == NULL)
		return;


	m_nSpinPosFire1 += GAUSS_GUN_SPIN_RATE_FIRE1;
	SetPoseParameter(GAUSS_GUN_SPIN_FIRE1, m_nSpinPosFire1);

	if (m_bCharging == true)
	{
		return;
	}


	if (m_bCharging == false)
	{
		WeaponSound(SINGLE);
		WeaponSound(SPECIAL2);

		//SendWeaponAnim( ACT_VM_PRIMARYATTACK );

		m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
		pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);

		Fire();

		m_flCoilMaxVelocity = 0.0f;
		m_flCoilVelocity = 1000.0f;
	}




	return;

}
//---------------------------------------------------------
//---------------------------------------------------------
//void CWeaponGauss::MakeTracer(const Vector &vecTracerSrc, const trace_t &tr, int iTracerType)
void CWeaponGauss::MakeTracer(void)
{
	//Vector vecTracerSrc;

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	Vector  startPos = pOwner->Weapon_ShootPosition();
	Vector  aimDir = pOwner->GetAutoaimVector(AUTOAIM_5DEGREES);

	Vector vecUp, vecRight;
	VectorVectors(aimDir, vecRight, vecUp);

	trace_t tr;
	Vector  endPos = startPos + (aimDir * MAX_TRACE_LENGTH);


	/*
			Vector vecShootOrigin2;  //The origin of the shot
		QAngle	angShootDir2;    //The angle of the shot

		//We need to figure out where to place the particle effect, so look up where the muzzle is
		GetAttachment( LookupAttachment( "muzzle" ), vecShootOrigin2, angShootDir2 );

		//pOperator->DoMuzzleFlash();
		DispatchParticleEffect( "striderbuster_break_b", vecShootOrigin2, angShootDir2);
		*/



	int iEntIndex = pOwner->entindex();

	if (g_pGameRules->IsMultiplayer())
	{
		iEntIndex = entindex();
	}

	int iAttachment = GetTracerAttachment();
	UTIL_ParticleTracer("tau_tracer", startPos, endPos, iEntIndex, iAttachment, false);
	UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", startPos, endPos, iEntIndex, iAttachment, false);
}

//---------------------------------------------------------
// MuzzleLight
//---------------------------------------------------------
void CWeaponGauss::MuzzleLight(void)
{
#ifdef CLIENT_DLL
	Vector vAttachment, vAng;
	QAngle angles;

	GetAttachment(1, vAttachment, angles); // set 1 instead "attachment"


	AngleVectors(angles, &vAng);
	vAttachment += vAng * 2;
	extern IVEfx* effects;
	dlight_t* dl = effects->CL_AllocDlight(index);
	dl->origin = vAttachment;
	dl->color.r = 252;
	dl->color.g = 238;
	dl->color.b = 128;
	dl->die = gpGlobals->curtime + 0.05f;
	dl->radius = random->RandomFloat(245.0f, 256.0f);
	dl->decay = 512.0f;
#endif
	DoMuzzleFlash();
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::Fire(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;
	//MakeTracer();
	m_bCharging = false;


	MuzzleLight();

#ifdef CLIENT_DLL
	DispatchParticleEffect("hl2mmod_muzzleflash_gauss", PATTACH_POINT_FOLLOW, pOwner->GetViewModel(), "muzzle", true);
#endif
#ifndef CLIENT_DLL
	DispatchParticleEffect("hl2mmod_tracer_gaussrifle_tracer_buggy2", PATTACH_POINT_FOLLOW, this, "muzzle", true);
#endif


	Vector  startPos = pOwner->Weapon_ShootPosition();
	Vector  aimDir = pOwner->GetAutoaimVector(AUTOAIM_5DEGREES);

	Vector vecUp, vecRight;
	VectorVectors(aimDir, vecRight, vecUp);

	float x, y, z;

	//Gassian spread
	do {
		x = random->RandomFloat(-0.5, 0.5) + random->RandomFloat(-0.5, 0.5);
		y = random->RandomFloat(-0.5, 0.5) + random->RandomFloat(-0.5, 0.5);
		z = x * x + y * y;
	} while (z > 1);


	Vector  endPos = startPos + (aimDir * MAX_TRACE_LENGTH);

	//Shoot a shot straight out
	trace_t tr;
	UTIL_TraceLine(startPos, endPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr);
#ifndef CLIENT_DLL
	ClearMultiDamage();
#endif

	CBaseEntity* pHit = tr.m_pEnt;
#ifndef CLIENT_DLL         
	CTakeDamageInfo dmgInfo(this, pOwner, sk_dmg_gauss.GetFloat(), DMG_SHOCK | DMG_BULLET);
#endif

	if (pHit != NULL)
	{
#ifndef CLIENT_DLL
		CalculateBulletDamageForce(&dmgInfo, m_iPrimaryAmmoType, aimDir, tr.endpos, 7.0f * 5.0f);
		pHit->DispatchTraceAttack(dmgInfo, aimDir, &tr);
#endif
	}

	if (tr.DidHitWorld())
	{
		float hitAngle = -DotProduct(tr.plane.normal, aimDir);

		if (hitAngle < 0.5f)
		{
			Vector vReflection;

			vReflection = 2.0 * tr.plane.normal * hitAngle + aimDir;

			startPos = tr.endpos;
			endPos = startPos + (vReflection * MAX_TRACE_LENGTH);

			//Draw beam to reflection point
			DrawBeam(tr.startpos, tr.endpos, 1.6, true);
			//DrawBeam2( tr.startpos, tr.endpos, 2.6, true );
			int iEntIndex = pOwner->entindex();

			if (g_pGameRules->IsMultiplayer())
			{
				iEntIndex = entindex();
			}

			int iAttachment = GetTracerAttachment();
			UTIL_ParticleTracer("tau_tracer", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
			UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
			CPVSFilter filter(tr.endpos);
			te->GaussExplosion(filter, 0.0f, tr.endpos, tr.plane.normal, 0);

			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");

			if (tr.DidHitWorld())
			{
				//UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussYellowGlow");
				UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
			}
			//Find new reflection end position
			UTIL_TraceLine(startPos, endPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr);

			if (tr.m_pEnt != NULL)
			{
#ifndef CLIENT_DLL
				dmgInfo.SetDamageForce(GetAmmoDef()->DamageForce(m_iPrimaryAmmoType) * vReflection);
				dmgInfo.SetDamagePosition(tr.endpos);
				tr.m_pEnt->DispatchTraceAttack(dmgInfo, vReflection, &tr);
#endif
			}

			//Connect reflection point to end
			DrawBeam(tr.startpos, tr.endpos, 0.4);
			//DrawBeam2( tr.startpos, tr.endpos, 0.8 );
			UTIL_ParticleTracer("tau_tracer", tr.startpos, tr.endpos, iAttachment, true);
			UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", tr.startpos, tr.endpos, iAttachment, true);
		}
		else
		{
			DrawBeam(tr.startpos, tr.endpos, 1.6, true);
			//DrawBeam2( tr.startpos, tr.endpos, 2.6, true );
			int iEntIndex = pOwner->entindex();

			if (g_pGameRules->IsMultiplayer())
			{
				iEntIndex = entindex();
			}

			int iAttachment = GetTracerAttachment();
			UTIL_ParticleTracer("tau_tracer", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
			UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
		}
	}
	else
	{
		DrawBeam(tr.startpos, tr.endpos, 1.6, true);
		//DrawBeam2( tr.startpos, tr.endpos, 2.6, true );
		int iEntIndex = pOwner->entindex();

		if (g_pGameRules->IsMultiplayer())
		{
			iEntIndex = entindex();
		}

		int iAttachment = GetTracerAttachment();
		UTIL_ParticleTracer("tau_tracer", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
		UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
	}
#ifndef CLIENT_DLL         
	ApplyMultiDamage();
#endif

	UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
	if (tr.DidHitWorld())
	{
		//UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussYellowGlow");
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
	}
	CPVSFilter filter(tr.endpos);
	te->GaussExplosion(filter, 0.0f, tr.endpos, tr.plane.normal, 0);

	m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;

	AddViewKick();

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	return;
}
//-----------------------------------------------------------------------------
// Purpose: Draw the first Carged Beam
//-----------------------------------------------------------------------------
void CWeaponGauss::ChargedFireFirstBeam(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	Vector  startPos = pOwner->Weapon_ShootPosition();
	Vector  aimDir = pOwner->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector  endPos = startPos + (aimDir * MAX_TRACE_LENGTH);

	//Shoot a shot straight out
	trace_t tr;
	UTIL_TraceLine(startPos, endPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr);
	startPos = tr.endpos;


	//Draw beam
	//DrawBeam( tr.startpos, tr.endpos, 9.6, true );
	DrawBeam2(tr.startpos, tr.endpos, 3.6, true);


	int iEntIndex = pOwner->entindex();

	if (g_pGameRules->IsMultiplayer())
	{
		iEntIndex = entindex();
	}

	int iAttachment = GetTracerAttachment();
	UTIL_ParticleTracer("tau_tracer", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);
	UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", tr.startpos, tr.endpos, iEntIndex, iAttachment, true);


	CPVSFilter filter(tr.endpos);
	te->GaussExplosion(filter, 0.0f, tr.endpos, tr.plane.normal, 0);

	UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
	if (tr.DidHitWorld())
	{
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
	}
	return;
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::SecondaryAttack(void)
{


	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return;


	/*
#ifdef CLIENT_DLL
	DispatchParticleEffect("hl2mmod_muzzleflash_gauss_charge", PATTACH_POINT_FOLLOW, pOwner->GetViewModel(), "spinner1", true);
#endif
#ifndef CLIENT_DLL
	DispatchParticleEffect("hl2mmod_muzzleflash_gauss_charge", PATTACH_POINT_FOLLOW, this, "spinner1", true);
#endif
	*/

	if (m_bCharging)
	{
		m_nSpinPos += GAUSS_GUN_SPIN_RATE;
		SetPoseParameter(GAUSS_GUN_SPIN, m_nSpinPos);
	}


	if (m_bCharging == false)
	{
		//Start looping animation
		SendWeaponAnim(ACT_VM_PULLBACK);
		//SendWeaponAnim( ACT_VM_PULLBACK_LOW );
		//Start looping sound
		if (m_sndCharge == NULL)
		{
			//EmitSound( "Weapon_Gauss.Special1" );
			//CPASAttenuationFilter filter( this );
			//m_sndCharge	= (CSoundEnvelopeController::GetController()).SoundCreate( filter, entindex(), CHAN_STATIC, "weapons/gauss/chargeloop.wav", ATTN_NORM );
			WeaponSound(SPECIAL1);
		}

		assert(m_sndCharge != NULL);
		if (m_sndCharge != NULL)
		{
			//(CSoundEnvelopeController::GetController()).Play( m_sndCharge, 1.0f, 50 );
			//(CSoundEnvelopeController::GetController()).SoundChangePitch( m_sndCharge, 250, 3.0f );
			//EmitSound( "Weapon_Gauss.Special1" );
			WeaponSound(SPECIAL1);
		}



		m_flChargeStartTime = gpGlobals->curtime;
		m_bCharging = true;
		m_bChargeIndicated = false;

		//Decrement power
		pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);
	}

	IncreaseCharge();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::IncreaseCharge(void)
{
	if (m_flNextChargeTime > gpGlobals->curtime)
		return;
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	//Check our charge time
	if ((gpGlobals->curtime - m_flChargeStartTime) > MAX_GAUSS_CHARGE_TIME)
	{
		//Notify the player they're at maximum charge
		if (m_bChargeIndicated == false)
		{
			WeaponSound(SPECIAL2);
			m_bChargeIndicated = true;
			//	SendWeaponAnim( ACT_VM_PULLBACK );
		}
		if ((gpGlobals->curtime - m_flChargeStartTime) > DANGER_GAUSS_CHARGE_TIME)
		{
			//Damage the player
			//WeaponSound( SPECIAL2 );
			WeaponSound(SINGLE);
			// Add DMG_CRUSH because we don't want any physics force
#ifndef CLIENT_DLL
			pOwner->TakeDamage(CTakeDamageInfo(this, this, 25, DMG_RADIATION | DMG_BULLET));
			color32 gaussDamage = { 255,128,0,128 };
			UTIL_ScreenFade(pOwner, gaussDamage, 0.2f, 0.2f, FFADE_IN);

			StopChargeSound();
			if (m_bCharging == true) {

				if (pOwner == NULL)
					return;
				SendWeaponAnim(ACT_VM_SECONDARYATTACK);

				StopChargeSound();
				StopSound("Weapon_Gauss.Special1");
				StopWeaponSound(SPECIAL1);

				m_flNextSecondaryAttack = gpGlobals->curtime + 3.0f;
				m_flNextPrimaryAttack = gpGlobals->curtime + 3.0f;

				AddViewKick();
			}

			m_bCharging = false;
			m_bChargeIndicated = false;

			if (m_sndCharge != NULL)
			{
				(CSoundEnvelopeController::GetController()).SoundFadeOut(m_sndCharge, 0.1f);
			}
			m_sndCharge = NULL;

			StopChargeSound();
#endif
			m_flNextChargeTime = gpGlobals->curtime + random->RandomFloat(0.5f, 2.5f);

		}
		return;
	}
	//Decrement power
	pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);
	//Make sure we can draw power
	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		ChargedFire();
		ChargedFireFirstBeam();
		return;
	}

	m_flNextChargeTime = gpGlobals->curtime + GAUSS_CHARGE_TIME;
	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::ChargedFire(void)
{


	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	MuzzleLight();
#ifdef CLIENT_DLL
	DispatchParticleEffect("hl2mmod_muzzleflash_gauss_alt", PATTACH_POINT_FOLLOW, pOwner->GetViewModel(), "muzzle", true);
#endif
#ifndef CLIENT_DLL
	DispatchParticleEffect("hl2mmod_tracer_gaussrifle_tracer_buggy2", PATTACH_POINT_FOLLOW, this, "muzzle", true);
#endif


	bool penetrated = false;

	//Play shock sounds
	WeaponSound(SINGLE);
	WeaponSound(SPECIAL2);

	SendWeaponAnim(ACT_VM_SECONDARYATTACK);

	StopChargeSound();
	StopSound("Weapon_Gauss.Special1");
	StopWeaponSound(SPECIAL1);


	m_bCharging = false;
	m_bChargeIndicated = false;


	m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;
	m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;
	//Shoot a shot straight
	Vector  startPos = pOwner->Weapon_ShootPosition();
	Vector  aimDir = pOwner->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector  endPos = startPos + (aimDir * MAX_TRACE_LENGTH);


	//Find Damage
	float flChargeAmount = (gpGlobals->curtime - m_flChargeStartTime) / MAX_GAUSS_CHARGE_TIME;
	//Clamp This
	if (flChargeAmount > 1.0f)
	{
		flChargeAmount = 1.0f;
	}
#ifndef CLIENT_DLL
	// float flDamage = sk_plr_max_dmg_gauss.GetFloat() + ( ( sk_plr_max_dmg_gauss.GetFloat() - sk_plr_max_dmg_gauss.GetFloat() ) * flChargeAmount );


	float flDamage2 = 3 + ((37 - 15) * flChargeAmount);
	float flDamage = 20 + ((90 - 15) * flChargeAmount);
#endif
	trace_t tr;
	UTIL_TraceLine(startPos, endPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr); //Trace from gun to wall

	UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
	if (tr.DidHitWorld())
	{
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
	}

#ifndef CLIENT_DLL
	//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
	RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif


#ifndef CLIENT_DLL
	ClearMultiDamage();
#endif

	UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
	if (tr.DidHitWorld())
	{
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
	}
#ifndef CLIENT_DLL
	// RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
	RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif


	CBaseEntity* pHit = tr.m_pEnt;

	if (tr.DidHitWorld()) {

		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
		if (tr.DidHitWorld())
		{
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
		}
		CPVSFilter filter(tr.endpos);
		te->GaussExplosion(filter, 0.0f, tr.endpos, tr.plane.normal, 0);

		Vector  testPos = tr.endpos + (aimDir * 128.0f);

		UTIL_TraceLine(testPos, tr.endpos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr); //Trace to backside of first wall

		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
		if (tr.DidHitWorld())
		{
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
		}
#ifndef CLIENT_DLL
		//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
		RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif


		if (tr.allsolid == false) {
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
			if (tr.DidHitWorld())
			{
				UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
				UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
			}
#ifndef CLIENT_DLL
			//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
			RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif

			penetrated = true;

			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
			if (tr.DidHitWorld())
			{
				UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
				UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
			}
#ifndef CLIENT_DLL
			//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
			RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif

		}

	}
	else if (pHit != NULL) {
#ifndef CLIENT_DLL
		// CTakeDamageInfo dmgInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK );
//		  CalculateBulletDamageForce( &dmgInfo, m_iPrimaryAmmoType, aimDir, tr.endpos );
		UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
		if (tr.DidHitWorld())
		{
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
			UTIL_ImpactTrace(&tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
		}
		//Do Direct damage to anything in our path
//		  pHit->DispatchTraceAttack( dmgInfo, aimDir, &tr );
#endif
	}
#ifndef CLIENT_DLL
	ApplyMultiDamage();
#endif


#ifndef CLIENT_DLL
	//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
	RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif

	Vector  newPos = tr.endpos + (aimDir * MAX_TRACE_LENGTH);
	QAngle  viewPunch;
	viewPunch.x = random->RandomFloat(-4.0f, -8.0f);
	viewPunch.y = random->RandomFloat(-0.25f, 0.25f);
	viewPunch.z = 0;
	pOwner->ViewPunch(viewPunch);

	// DrawBeam( startPos, tr.endpos, 9.6, true ); //Draw beam from gun through first wall.
#ifndef CLIENT_DLL
	Vector	recoilForce = pOwner->BodyDirection3D() * -(flDamage2 * 15.0f);
	recoilForce[2] += 128.0f;

	pOwner->ApplyAbsVelocityImpulse(recoilForce);
#endif
	CPVSFilter filter(tr.endpos);
	te->GaussExplosion(filter, 0.0f, tr.endpos, tr.plane.normal, 0);

#ifndef CLIENT_DLL
	//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ),tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
	RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif


	if (penetrated == true) {

		trace_t beam_tr;
		Vector vecDest = tr.endpos + aimDir * MAX_TRACE_LENGTH;
		UTIL_TraceLine(tr.endpos, vecDest, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &beam_tr); //Traces from back of first wall to second wall


#ifndef CLIENT_DLL
	 // float flDamage = sk_plr_max_dmg_gauss.GetFloat() + ( ( sk_plr_max_dmg_gauss.GetFloat() - sk_plr_max_dmg_gauss.GetFloat() ) * flChargeAmount );


		float flDamage = 20 + ((90 - 15) * flChargeAmount);
#endif



		for (int i = 0; i < 0; i++) {

			UTIL_TraceLine(beam_tr.endpos + aimDir * 128.0f, beam_tr.endpos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &beam_tr); //Traces To back of second wall


			UTIL_ImpactTrace(&beam_tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
			if (tr.DidHitWorld())
			{
				UTIL_ImpactTrace(&beam_tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
				UTIL_ImpactTrace(&beam_tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
			}
#ifndef CLIENT_DLL
			//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ), beam_tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
			RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif



		}
		//DrawBeam( tr.endpos, beam_tr.endpos, 9.6, false );
		DrawBeam2(tr.endpos, beam_tr.endpos, 3.6, false);
		DoWallBreak(tr.endpos, newPos, aimDir, &tr, pOwner, true);


		int iEntIndex = pOwner->entindex();

		if (g_pGameRules->IsMultiplayer())
		{
			iEntIndex = entindex();
		}

		int iAttachment = GetTracerAttachment();
		UTIL_ParticleTracer("tau_tracer", tr.endpos, beam_tr.endpos, iEntIndex, iAttachment, true);
		UTIL_ParticleTracer("hl2mmod_tracer_gaussrifle_tracer_buggy1", tr.endpos, beam_tr.endpos, iEntIndex, iAttachment, true);


		UTIL_ImpactTrace(&beam_tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
		if (tr.DidHitWorld())
		{
			UTIL_ImpactTrace(&beam_tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
			UTIL_ImpactTrace(&beam_tr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
		}
#ifndef CLIENT_DLL
		//RadiusDamage( CTakeDamageInfo( this, pOwner, sk_plr_max_dmg_gauss.GetFloat(), DMG_SHOCK ), beam_tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner );
		RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), tr.endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif

		return;
	}

}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponGauss::DidPunchThrough(trace_t* tr)
{
	if (tr->DidHitWorld() && tr->surface.flags != SURF_SKY && ((tr->startsolid == false) || (tr->startsolid == true && tr->allsolid == false)))
		return true;
	return false;
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::DoWallBreak(Vector startPos, Vector endPos, Vector aimDir, trace_t* ptr, CBasePlayer* pOwner, bool m_bBreakAll) {
	trace_t* temp = ptr;
	if (m_bBreakAll) {
		Vector tempPos = endPos;
		Vector beamStart = startPos;
		int x = 0;
		while (DidPunchThrough(ptr)) {
			temp = ptr;
			if (x == 0) {
				UTIL_TraceLine(startPos, tempPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, ptr);
				x = 1;
			}
			else {
				UTIL_TraceLine(endPos, startPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, ptr);
				x = 0;
			}
			if (ptr->DidHitWorld() && ptr->surface.flags != SURF_SKY) {
				UTIL_ImpactTrace(ptr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "ImpactGauss");
				UTIL_ImpactTrace(ptr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "GaussRedGlow");
				UTIL_ImpactTrace(ptr, GetAmmoDef()->DamageType(m_iPrimaryAmmoType), "HelicopterImpact");
			}
			startPos = ptr->endpos;
			tempPos = ptr->endpos + (aimDir * MAX_TRACE_LENGTH + aimDir * 128.0f);


			//Find Damage
			float flChargeAmount = (gpGlobals->curtime - m_flChargeStartTime) / MAX_GAUSS_CHARGE_TIME;
			//Clamp This
			if (flChargeAmount > 1.0f)
			{
				flChargeAmount = 1.0f;
			}
#ifndef CLIENT_DLL
			// float flDamage = sk_plr_max_dmg_gauss.GetFloat() + ( ( sk_plr_max_dmg_gauss.GetFloat() - sk_plr_max_dmg_gauss.GetFloat() ) * flChargeAmount );


			float flDamage = 20 + ((90 - 15) * flChargeAmount);
#endif



#ifndef CLIENT_DLL
			RadiusDamage(CTakeDamageInfo(this, pOwner, flDamage, DMG_SHOCK), ptr->endpos, 90.0f, CLASS_PLAYER_ALLY, pOwner);
#endif



		}
		//DrawBeam( beamStart, ptr->startpos, 4.0, false );






	}
	else {
		UTIL_TraceLine(startPos, endPos, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, ptr); //Trace from gun to wall
	}

	if (!DidPunchThrough(ptr)) {
		ptr = temp;
		return;
	}
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::DrawBeam(const Vector& startPos, const Vector& endPos, float width, bool useMuzzle)
{

#ifndef CLIENT_DLL

	//Draw the main beam shaft
	CBeam* m_pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE, width);

	if (useMuzzle)
	{
		m_pBeam->PointEntInit(endPos, this);
		m_pBeam->SetEndAttachment(LookupAttachment("Muzzle"));
		m_pBeam->SetWidth(width / 4.0f);
		m_pBeam->SetEndWidth(width);
		m_pBeam->RelinkBeam();
	}

	else
	{
		m_pBeam->SetStartPos(startPos);
		m_pBeam->SetEndPos(endPos);
		m_pBeam->SetWidth(width);
		m_pBeam->SetEndWidth(width / 4.0f);
		m_pBeam->RelinkBeam();
	}
	m_pBeam->SetBrightness(255);
	m_pBeam->SetColor(255, 145 + random->RandomInt(-16, 16), 0);
	m_pBeam->LiveForTime(0.1f);
	m_pBeam->RelinkBeam();
	//Draw electric bolts along shaft
	for (int i = 0; i < 3; i++)
	{
		m_pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE, (width / 2.0f) + i);

		if (useMuzzle)
		{
			m_pBeam->PointEntInit(endPos, this);
			m_pBeam->SetEndAttachment(LookupAttachment("Muzzle"));
			m_pBeam->RelinkBeam();
		}
		else
		{
			m_pBeam->SetStartPos(startPos);
			m_pBeam->SetEndPos(endPos);
			m_pBeam->RelinkBeam();
		}
		m_pBeam->SetBrightness(random->RandomInt(64, 255));
		m_pBeam->SetColor(255, 255, 150 + random->RandomInt(0, 64));
		m_pBeam->LiveForTime(0.1f);
		m_pBeam->SetNoise(1.6f * i);
		m_pBeam->SetEndWidth(0.1f);
		m_pBeam->RelinkBeam();
	}
#endif
	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::DrawBeam2(const Vector& startPos, const Vector& endPos, float width, bool useMuzzle)
{

#ifndef CLIENT_DLL

	//Draw the main beam shaft
	CBeam* m_pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE2, width);

	if (useMuzzle)
	{
		m_pBeam->PointEntInit(endPos, this);
		m_pBeam->SetEndAttachment(LookupAttachment("Muzzle"));
		m_pBeam->SetWidth(width / 4.0f);
		m_pBeam->SetEndWidth(width);
		m_pBeam->RelinkBeam();
	}

	else
	{
		m_pBeam->SetStartPos(startPos);
		m_pBeam->SetEndPos(endPos);
		m_pBeam->SetWidth(width);
		m_pBeam->SetEndWidth(width / 4.0f);
		m_pBeam->RelinkBeam();
	}
	m_pBeam->SetBrightness(255);
	m_pBeam->SetColor(255, 145 + random->RandomInt(-16, 16), 0);
	m_pBeam->LiveForTime(0.1f);
	m_pBeam->RelinkBeam();
	//Draw electric bolts along shaft
	for (int i = 0; i < 3; i++)
	{
		m_pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE2, (width / 2.0f) + i);

		if (useMuzzle)
		{
			m_pBeam->PointEntInit(endPos, this);
			m_pBeam->SetEndAttachment(LookupAttachment("Muzzle"));
			m_pBeam->RelinkBeam();
		}
		else
		{
			m_pBeam->SetStartPos(startPos);
			m_pBeam->SetEndPos(endPos);
			m_pBeam->RelinkBeam();
		}
		m_pBeam->SetBrightness(random->RandomInt(64, 255));
		m_pBeam->SetColor(255, 255, 150 + random->RandomInt(0, 64));
		m_pBeam->LiveForTime(0.1f);
		m_pBeam->SetNoise(1.6f * i);
		m_pBeam->SetEndWidth(0.1f);
		m_pBeam->RelinkBeam();
	}
#endif
	return;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::AddViewKick(void)
{
	//Get the view kick
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	QAngle	viewPunch;

	viewPunch.x = random->RandomFloat(-0.5f, -0.2f);
	viewPunch.y = random->RandomFloat(-0.5f, 0.5f);
	viewPunch.z = 0;

	pPlayer->ViewPunch(viewPunch);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::ItemPostFrame(void)
{
	//Get the view kick
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (pPlayer == NULL)
		return;

	if (pPlayer->m_afButtonReleased & IN_ATTACK2)
	{
		if (m_bCharging)
		{
			ChargedFire();
			ChargedFireFirstBeam();
		}
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponGauss::StopChargeSound(void)
{
	if (m_sndCharge != NULL)
	{
		(CSoundEnvelopeController::GetController()).SoundFadeOut(m_sndCharge, 0.1f);
	}
	m_sndCharge = NULL;

}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSwitchingTo - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponGauss::Holster(CBaseCombatWeapon* pSwitchingTo)
{

	if (m_bCharging)
	{
		StopChargeSound();
		m_bCharging = false;
		m_bChargeIndicated = false;
		StopSound("Weapon_Gauss.Special1");
		StopWeaponSound(SPECIAL1);
	}






	StopChargeSound();
	StopSound("Weapon_Gauss.Special1");
	StopWeaponSound(SPECIAL1);
	return BaseClass::Holster(pSwitchingTo);
}


