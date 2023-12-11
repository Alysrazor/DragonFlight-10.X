#include "ScriptMgr.h"
#include "ScriptedCreature.h"

using namespace std;

enum Spells : uint32
{
	// Holy Phase
	SPELL_BULWARK_OF_ORDER = 387740,
    SPELL_HOLY_FORM = 46565,
    SPELL_HOLY_SMITE = 62443,
	SPELL_HOLY_WRATH = 343154, // Hechizo de medio daño 15 - 20s
    SPELL_HOLY_WRATH_ENRAGE = 227823, // Enrage
    SPELL_TITANIC_BULKWARK = 235028, // Hechizo largo de protección cada 1m

    // Holy Elemental
    SPELL_HOLY_FLARE = 242715,

    // Shadow Phase
    SPELL_SHADOW_BARRIER = 63710,
	SPELL_SHADOWFORM = 284439, // 10% Shadow Dmg Done - 10% Physical Damage Taken
	SPELL_SHADOWFORM_HEROIC = 35194, // 20% Shadow Dmg Done - 20% Physical Damage Taken
	SPELL_SHADOWFORM_MYTHIC = 22917, // 40% Shadow Dmg Done - 40% Physical Damage Taken
	
};

enum Events : uint8
{
    //Holy Phase
    EVENT_HOLY_SMITE,
    EVENT_HOLY_WRATH,
    EVENT_HOLY_WRATH_ENRAGE,

    // Shadow Phase
    EVENT_SHADOW_BARRIER,
};

enum EventGroup : uint8
{
    EVENT_GROUP_HOLY,
    EVENT_GROUP_SHADOW
};

enum Phase : uint8
{
    PHASE_ONE = 1,
    INTERPHASE_SHADOW,
    PHASE_TWO,
    INTERPHASE_HOLY
};

struct custom_boss_high_inquisitor_whitemane : public WorldBossAI
{
public:
    custom_boss_high_inquisitor_whitemane(Creature* p_Creature) : WorldBossAI(p_Creature), m_KillYellTimer(Seconds(0))
    {

    }

    void Reset() override
    {
        WorldBossAI::Reset();
        m_Events.Reset();
        m_KillYellTimer.Reset(Seconds(0));
    }

    void JustEngagedWith(Unit* p_Who) override
    {
        if (p_Who->GetTypeId() == TYPEID_PLAYER)
            me->Say("[PH] SAY_AGGRO", Language::LANG_UNIVERSAL);
        else
            me->Kill(me, p_Who);

        m_Events.SetPhase(PHASE_ONE);
    }

    void KilledUnit(Unit* p_Victim) override
    {
        if (p_Victim->GetTypeId() == TYPEID_PLAYER)
        {
            if (m_KillYellTimer.Passed())
            {
                me->Say("[PH] SAY_KILL", Language::LANG_UNIVERSAL);
                m_KillYellTimer.Reset(Seconds(5));
            }            
        }
    }

    void JustDied(Unit* p_Killer) override
    {
        WorldBossAI::JustDied(p_Killer);
    }

    void DamageTaken(Unit* p_Attacker, uint32& p_Damage, DamageEffectType p_DamageEffectType, SpellInfo const* p_SpellInfo = nullptr) override
    {
        if (p_Attacker->GetTypeId() == TYPEID_PLAYER || p_Attacker->GetGUID().IsPet())
        {
            if (me->HealthBelowPctDamaged(67, p_Damage) && m_Events.IsInPhase(PHASE_ONE))
            {
                m_Events.SetPhase(INTERPHASE_HOLY);
            }
        }
    }

    void EnterEvadeMode(EvadeReason p_Reason) override
    {
        WorldBossAI::EnterEvadeMode();
        me->DespawnOrUnsummon(0s, 30s);
    }

private:
    EventMap m_Events;
    TaskScheduler m_Scheduler;
    TimeTracker m_KillYellTimer;

    void ScheduleHolyEvents()
    {
        m_Scheduler.Schedule(3s, [this](TaskContext /*context*/)
        {
                me->Say("[PH] HOLY_PHASE", Language::LANG_UNIVERSAL);
                me->AddAura(SPELL_HOLY_FORM, me);
                m_Events.SetPhase(Phase::PHASE_ONE);
                m_Events.ScheduleEvent(EVENT_HOLY_SMITE, Seconds(4), 0, PHASE_ONE);
        });
    }

    void PrepareHolyInterphase()
    {
        m_Events.CancelEventGroup(EVENT_GROUP_HOLY);
    }

    void ScheduleShadowEvents()
    {
        me->Say("NOT YET IMPLEMENTED", Language::LANG_UNIVERSAL);
    }

    void PrepareShadowInterphase()
    {
        m_Events.CancelEventGroup(EVENT_GROUP_SHADOW);
    }
};
