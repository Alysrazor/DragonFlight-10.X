#include "AccountMgr.h"
#include "Chat.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

enum ChatOptions
{
    NPC_GOSSIP_MENU = 1010000 // Version 10 Patch 1 Bugfix 0 ID 000
};

enum DisplayIDs
{
    DISPLAY_ALEXSTRAZSA = 28227
};

struct npc_custom_gossip : public ScriptedAI
{
    npc_custom_gossip(Creature* p_Creature) : ScriptedAI(p_Creature)
    {
        p_Creature->SetFaction(35);
        p_Creature->SetImmuneToNPC(true);
    }

    /*
        Preparamos el men� de di�logo para el jugador
    */
    bool OnGossipHello(Player* p_Player) override
    {
        InitGossipMenuFor(p_Player, NPC_GOSSIP_MENU);

        // Aqu� ir�an condiciones en caso de que fueran necesarias.

        p_Player->TalkedToCreature(me->GetEntry(), me->GetGUID());
        SendGossipMenuFor(p_Player, p_Player->GetGossipTextId(me), me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* p_Player, uint32 /*p_MenuID*/, uint32 p_GossipListID) override
    {
        uint32 const l_Action = p_Player->PlayerTalkClass->GetGossipOptionAction(p_GossipListID);
        ClearGossipMenuFor(p_Player);
        // Aqu� ir�an las condiciones para cada selecci�n del men�.

        CloseGossipMenuFor(p_Player);
        
        // Aqu� ir�a un switch con las acciones
        switch (l_Action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                p_Player->SetDisplayId(DISPLAY_ALEXSTRAZSA);
                me->Say("Listo, ya puedes disfrutar de tu disfraz de Alexstrazsa.", LANG_UNIVERSAL);
                break;
            default:
                p_Player->RestoreDisplayId();
        }

        return true;
    }
};

void AddSC_npc_custom_gossip()
{
    RegisterCreatureAI(npc_custom_gossip);
}
