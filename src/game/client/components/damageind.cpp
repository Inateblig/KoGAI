/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/demo.h>
#include <engine/graphics.h>
#include <game/generated/client_data.h>
#include <game/generated/protocol.h>

#include "damageind.h"
#include <game/client/render.h>

#include <game/client/gameclient.h>

CDamageInd::CDamageInd()
{
	m_Lastupdate = 0;
	m_NumItems = 0;
}

CDamageInd::CItem *CDamageInd::CreateI()
{
	if(m_NumItems < MAX_ITEMS)
	{
		CItem *p = &m_aItems[m_NumItems];
		m_NumItems++;
		return p;
	}
	return 0;
}

void CDamageInd::DestroyI(CDamageInd::CItem *pItem)
{
	m_NumItems--;
	*pItem = m_aItems[m_NumItems];
}

void CDamageInd::Create(vec2 Pos, vec2 Dir)
{
	CItem *pItem = CreateI();
	if(pItem)
	{
		pItem->m_Pos = Pos;
		pItem->m_StartTime = LocalTime();
		pItem->m_Dir = Dir * -1;
		pItem->m_StartAngle = (random_float() - 1.0f) * 2.0f * pi;
	}
}

void CDamageInd::OnRender()
{
	Graphics()->TextureSet(GameClient()->m_GameSkin.m_aSpriteStars[0]);
	static float s_LastLocalTime = LocalTime();
	for(int i = 0; i < m_NumItems;)
	{
		if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		{
			const IDemoPlayer::CInfo *pInfo = DemoPlayer()->BaseInfo();
			if(pInfo->m_Paused)
				m_aItems[i].m_StartTime += LocalTime() - s_LastLocalTime;
			else
				m_aItems[i].m_StartTime += (LocalTime() - s_LastLocalTime) * (1.0f - pInfo->m_Speed);
		}
		else
		{
			if(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_PAUSED)
				m_aItems[i].m_StartTime += LocalTime() - s_LastLocalTime;
		}

		float Life = 0.75f - (LocalTime() - m_aItems[i].m_StartTime);
		if(Life < 0.0f)
			DestroyI(&m_aItems[i]);
		else
		{
			vec2 Pos = mix(m_aItems[i].m_Pos + m_aItems[i].m_Dir * 75.0f, m_aItems[i].m_Pos, clamp((Life - 0.60f) / 0.15f, 0.0f, 1.0f));
			Graphics()->SetColor(1.0f, 1.0f, 1.0f, Life / 0.1f);
			Graphics()->QuadsSetRotation(m_aItems[i].m_StartAngle + Life * 2.0f);
			Graphics()->RenderQuadContainerAsSprite(m_DmgIndQuadContainerIndex, 0, Pos.x, Pos.y);
			i++;
		}
	}
	s_LastLocalTime = LocalTime();

	Graphics()->QuadsSetRotation(0);
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);
}

void CDamageInd::OnInit()
{
	Graphics()->QuadsSetRotation(0);
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);

	m_DmgIndQuadContainerIndex = Graphics()->CreateQuadContainer(false);
	float ScaleX, ScaleY;
	RenderTools()->GetSpriteScale(SPRITE_STAR1, ScaleX, ScaleY);
	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	RenderTools()->QuadContainerAddSprite(m_DmgIndQuadContainerIndex, 48.f * ScaleX, 48.f * ScaleY);
	Graphics()->QuadContainerUpload(m_DmgIndQuadContainerIndex);
}

void CDamageInd::Reset()
{
	for(int i = 0; i < m_NumItems;)
	{
		DestroyI(&m_aItems[i]);
	}
}
